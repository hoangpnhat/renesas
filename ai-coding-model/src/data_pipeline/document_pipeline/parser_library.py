# Databricks notebook source
# MAGIC %md
# MAGIC ## Parser Library
# MAGIC
# MAGIC This notebook implements a library of parsing tools for PDF documents. It should be `%run` as the first cell inside the `02_parse_docs` notebook. The `parser_factory` function implemented below can then be used to obtain a UDF for the parsing method specified in the `pipeline_config` of the `00_config` notebook.
# MAGIC
# MAGIC ### Adding a New Parser
# MAGIC To add a new parser, follow these steps (refer to the provided example in the README):
# MAGIC - Ensure all required dependencies are installed in the next cells.
# MAGIC - Add another section in this notebook and implement the parsing function.
# MAGIC   - Name the function `parse_bytes_<method_name>`.
# MAGIC   - Ensure the output of the function complies with the `ParserReturnValue` class defined below to ensure compatibility with Spark UDFs.
# MAGIC - Add your new method to the `parser_factory` function defined below.
# MAGIC - For testing and development, include a simple testing function that loads the `test_data/test-document.pdf` file and asserts successful parsing.
# MAGIC

# COMMAND ----------

# MAGIC %md
# MAGIC ## Imports

# COMMAND ----------

import io
from typing import List, Dict, Any, Tuple, Optional, TypedDict
import warnings
from functools import partial
import os


import pyspark.sql.functions as func
from pyspark.sql.types import (
    StructType,
    StringType,
    StructField,
    MapType,
    ArrayType,
    IntegerType,
)

# COMMAND ----------

parser_debug_flag = False

# COMMAND ----------

# Use optimizations if available
dbr_majorversion = int(
    spark.conf.get("spark.databricks.clusterUsageTags.sparkVersion").split(".")[0]
)
if dbr_majorversion >= 14:
    spark.conf.set("spark.sql.execution.pythonUDF.arrow.enabled", True)

# COMMAND ----------

# MAGIC %md
# MAGIC The next cell defines the return type of the parser functions. The `ParserReturnValue` class serves as a type hint for the parser functions and must match the Spark schema, which is defined as the `returnType` of the UDF inside the `get_parser_udf` function.
# MAGIC
# MAGIC This function is used in the `parser_factory` below to convert any of the configured parser functions into a Spark UDF, which can be applied to the binary PDFs at scale.
# MAGIC
# MAGIC __Note__: The main output type is a `MapType`. This means that you can return any key-value pair from your parser in addition to your parsed text. For instance, this could include the total number of pages.
# MAGIC
# MAGIC If you require a more complex output format, such as individual pages along with their respective page numbers, you would need to choose a different schema. For example:
# MAGIC
# MAGIC
# MAGIC ```Python
# MAGIC class ParserReturnValue(TypedDict):
# MAGIC     OUTPUT_FIELD_NAME: List[Dict[str, str]]
# MAGIC     STATUS_FIELD_NAME: str
# MAGIC
# MAGIC returnType = StructType(
# MAGIC     [
# MAGIC         StructField(
# MAGIC             OUTPUT_FIELD_NAME,
# MAGIC             ArrayType(
# MAGIC                 StructType(
# MAGIC                     [
# MAGIC                         StructField("page_number", IntegerType()),
# MAGIC                         StructField("parsed_content", StringType()),
# MAGIC                     ]
# MAGIC                 )
# MAGIC             ),
# MAGIC             nullable=True,
# MAGIC         ),
# MAGIC         StructField(STATUS_FIELD_NAME, StringType(), nullable=True),
# MAGIC     ]
# MAGIC )
# MAGIC ```

# COMMAND ----------

# DBTITLE 1,Define Parser Output
OUTPUT_FIELD_NAME = "doc_parsed_contents"
STATUS_FIELD_NAME = "parser_status"


class ParserReturnValue(TypedDict):
    OUTPUT_FIELD_NAME: str
    STATUS_FIELD_NAME: str


def get_parser_udf(parser_function):
    """Return a Spark UDF of the specified parsing function

    The return type matches the structure defined in the
    ParserReturnValue class
    """
    parser_udf = func.udf(
        parser_function,
        returnType=StructType(
            [
                StructField(
                    OUTPUT_FIELD_NAME,
                    ArrayType(
                        StructType(
                            [
                                StructField("chapter", StringType()),
                                StructField("content", StringType()),
                            ]
                        )
                    ),
                    nullable=True,
                ),
                StructField(STATUS_FIELD_NAME, StringType(), nullable=True),
            ]
        ),
    )
    return parser_udf


# COMMAND ----------

# MAGIC %md
# MAGIC The `parser_factory` defined below assigns abbreviated names to the parsers and facilitates the configuration of different parsers through the `00_config` notebook. Any parser functions registered in the factory can be specified as the parser to be applied to the PDF binaries. The `parser_factory` function retrieves the specified parser from the configuration and returns the corresponding Spark UDF.

# COMMAND ----------

# DBTITLE 1,Define Parser Factory


def parser_factory(pipeline_config):

    # register all potential chunking methods and return respective UDF
    # different parser methods will require different parameters so we
    # use functools.partial to initialize them

    # Register all potential parsers and return as UDF
    if pipeline_config.get("parser").get("name") == "azure_doc_intelligence":
        print("setting up azure doc intelligence parser")
        return get_parser_udf(parse_bytes_adi)
    else:
        raise ValueError(
            f"The {pipeline_config.get('parser')} parser is not implemented. Choose a different one from the ./parsers notebook"
        )


# COMMAND ----------

# MAGIC %md
# MAGIC # Parsers

# COMMAND ----------

# MAGIC %md
# MAGIC ## Azure Document Intelligence
# MAGIC This is an Azure Service which requires authentication. You therefore have to provide:
# MAGIC - The endpoint url assigned to the `adi_endpoint` variable
# MAGIC - The authentication key assigned to the `adi_key` variable
# MAGIC
# MAGIC The [Azure documentation explains](https://learn.microsoft.com/en-us/azure/ai-services/document-intelligence/create-document-intelligence-resource?view=doc-intel-4.0.0#get-endpoint-url-and-keys) how to get both the endpoint and the key. It is recommended to store them in a [secret scope](https://docs.databricks.com/en/security/secrets/secret-scopes.html).
# MAGIC
# MAGIC *Note, the free tier will only parse 2 pages per request.*

# COMMAND ----------

import re


def extract_pages_and_table_info(result):
    data = result

    text_elements = []
    paragraphs = data.get("paragraphs", [])
    tables = data.get("tables", [])
    sections = data.get("sections", [])
    # Parsing tables to identify their text elements from paragraphs
    table_dict = {}
    tables_replaced = []
    for idx, table in enumerate(tables):
        table_text = ""
        row_count = table.get("rowCount", 0)
        col_count = table.get("columnCount", 0)
        cells = table.get("cells", [])
        # Using a 2D list to construct the table
        table_structure = [[""] * col_count for _ in range(row_count)]
        for cell in cells:
            if "content" in cell:
                row_idx = cell.get("rowIndex", -1)
                col_idx = cell.get("columnIndex", -1)
                table_structure[row_idx][col_idx] = cell["content"]
                # adding table information to pargraph list
                elem = cell.get("elements")
                if elem:
                    for para in elem:
                        if "paragraph" in para:
                            print(para)
                            match = re.search(r"(\d+)$", para)
                            if match:
                                id = int(match.group(0))
                                print(id)
                                paragraphs[id]["table"] = idx
        # Converting table structure to markdown
        header_row = "| " + " | ".join(table_structure[0]) + " |\n"
        separator_row = "| " + " | ".join(["---"] * col_count) + " |\n"
        table_text += header_row + separator_row
        for row in table_structure[1:]:
            table_text += "| " + " | ".join(row) + " |\n"
        table_dict[idx] = table_text

    text_elements_by_page = []
    tables_inserted = []
    current_chunk_elements = []  # can also be a letter

    # chunk by pages
    for paragraph in paragraphs:
        # do not include page footers
        role = paragraph.get("role", None)
        if role == "pageFooter":
            continue
        if role == "pageNumber":
            text_elements_by_page.append(
                {
                    "pageNumber": paragraph["content"],
                    "pageContent": "\n\n".join(current_chunk_elements),
                }
            )
            current_chunk_elements = []

        else:
            if "table" not in paragraph:
                current_chunk_elements.append(paragraph["content"])
            else:
                table_idx = paragraph["table"]
                if table_idx not in tables_inserted:
                    tables_inserted.append(table_idx)
                    markdown_table = table_dict.get(table_idx)
                    current_chunk_elements.append(markdown_table)
    return text_elements_by_page


# COMMAND ----------


def extract_chapters(text_elements_by_page):
    import re

    def contains_digit(string):
        return bool(re.search(r"\d", string))

    text_elements_by_chapter = []
    current_chunk_elements = []
    previous_chapter = 0

    for idx, text_element in enumerate(text_elements_by_page):
        page_number = text_element.get("pageNumber")
        page_content = text_element.get("pageContent")

        # extract_chapter
        if not contains_digit(page_number):  # roman numbering in introduction
            current_chapter = 0
        else:
            current_chapter = (
                page_number.split("-")[0]
                if "-" in page_number
                else text_elements_by_page[idx - 1].get("pageNumber").split("-")[0]
            )  # bug where some page numbers are not extracted properly. we default to using the previous page number to get the chapter
        if current_chapter != previous_chapter:
            text_elements_by_chapter.append(
                {
                    "chapter": previous_chapter,
                    "content": "\n\n".join(current_chunk_elements),
                }
            )
            current_chunk_elements = []

        current_chunk_elements.append(page_content)
        previous_chapter = current_chapter
    return text_elements_by_chapter


def combine_into_document(text_elements_by_page):
    content = ""
    for page in text_elements_by_page:
        content = (
            content
            + f"CONTENT OF PAGE {page['pageNumber']}: \n"
            + page["pageContent"]
            + "\n"
        )
    return content


# COMMAND ----------

from azure.ai.documentintelligence import DocumentIntelligenceClient
from azure.core.credentials import AzureKeyCredential
from azure.ai.documentintelligence.models import AnalyzeResult


try:

    adi_endpoint = os.getenv("ADI_ENDPOINT")
    adi_key = os.getenv("ADI_KEY")

    document_intelligence_client = DocumentIntelligenceClient(
        endpoint=adi_endpoint,
        credential=AzureKeyCredential(adi_key),
        api_version="2024-07-31-preview",
    )
except Exception as e:
    print("Failed to load Azure Doc Intelligence, make sure your secret is valid")
    print(e)


# TODO: Check different failure modes
def parse_bytes_adi(
    raw_doc_contents_bytes: bytes,
    # df_raw_bronze_path: str,
) -> ParserReturnValue:
    try:
        import io

        # df_raw_bronze = spark.read.format("delta").load(df_raw_bronze_path)
        # raw_doc_contents_bytes = df_raw_bronze.select("content").collect()[0][0]
        pdf_bytes = io.BytesIO(raw_doc_contents_bytes)
        poller = document_intelligence_client.begin_analyze_document(
            "prebuilt-layout",
            analyze_request=pdf_bytes,
            content_type="application/octet-stream",
            output_content_format="markdown",
        )
        print("Analyzing PDF ...")
        result: AnalyzeResult = poller.result()
        print("Extracting pages and table information")
        text_elements_by_page = extract_pages_and_table_info(result)
        print("extracting chapters")
        # text_elements_by_chapter = extract_chapters(
        #     text_elements_by_page
        # )  # list of [{"page_number":"", "content":""}]
        # print(f"sample chapter: {text_elements_by_chapter[0]}")
        text_elements_by_document = combine_into_document(text_elements_by_page)
        output = text_elements_by_document

        return {
            OUTPUT_FIELD_NAME: output,
            STATUS_FIELD_NAME: "SUCCESS",
        }
    # TODO: Be more specific about the exception
    except Exception as e:
        warnings.warn(f"Exception {e} has been thrown during parsing")
        return {
            OUTPUT_FIELD_NAME: {"chapter": "", "content": ""},
            STATUS_FIELD_NAME: f"ERROR: {e}",
        }
