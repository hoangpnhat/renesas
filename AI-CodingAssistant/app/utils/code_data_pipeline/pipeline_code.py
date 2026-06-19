# pipeline.py

import process_code_data,app.utils.code_data_pipeline.embed_old as embed_old
def run_pipeline():
    # Step 1: Process the code data
    processed_data = process_code_data.main()  # Call the main function or relevant processing function
    
    # Step 2: Embed the processed data
    embeddings = embed_old.main()  # Assuming the embedding script has a main function to process the data
    
    # Step 3: Output or use the embeddings as needed
    print("Embedding completed!")
    return embeddings

if __name__ == "__main__":
    run_pipeline()
