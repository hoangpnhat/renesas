import React, { PropsWithChildren, useMemo, useRef } from "react";
import { ReactMarkdownProps } from "../typings/component.props";
import ReactMarkdown from "react-markdown";
import { ReferenceSection } from "../pages/chat/components/ReferencesSection";
import { RetrievedDocs } from "../typings/request";
import { TypingText } from "./TypingText";

import { uniqueObjectsByKey } from "../utils/utilities.ts";
import QuestionAnswerOutlinedIcon from "@mui/icons-material/QuestionAnswerOutlined";

import { ROLE } from "../constants/common.ts";
import { useTranslation } from "react-i18next";
import { CommonCopyCodeBlock } from "./markdown/CommonCopyCodeBlock.tsx";
import { NormalComponents } from "react-markdown/lib/complex-types";
import { SpecialComponents } from "react-markdown/lib/ast-to-react";

const Markdown = ({ markdownContent }: { markdownContent: string }) => {
  const component:
    | Partial<
        Omit<NormalComponents, keyof SpecialComponents> & SpecialComponents
      >
    | undefined = useMemo(
    () => ({
      code({ inline, className, children, ...props }) {
        const match = /language-(\w+)/.exec(className || "");
        return !inline && match ? (
          <CommonCopyCodeBlock className={className as string} {...props}>
            {children}
          </CommonCopyCodeBlock>
        ) : (
          <code className={className} {...props}>
            {children}
          </code>
        );
      },
    }),
    [],
  );
  return (
    <ReactMarkdown components={component}>{markdownContent}</ReactMarkdown>
  );
};
export const CommonMarkdown = ({
  message,
  hasTyping,
}: PropsWithChildren<ReactMarkdownProps>) => {
  const ref = useRef(null);
  const { t } = useTranslation();
  const transformRefItems = useMemo(() => {
    return message.retrieved_docs && message.retrieved_docs?.length > 0
      ? Array.from(
          uniqueObjectsByKey<RetrievedDocs>(
            message.retrieved_docs as RetrievedDocs[],
            "doc_uri",
          ),
        )
      : [];
  }, [message.retrieved_docs]);
  return (
    <React.Fragment>
      <section ref={ref} className="markdown-section w-full mt-1">
        {message.role === ROLE.ASSISTANT && (
          <span className="flex my-2">
            <QuestionAnswerOutlinedIcon sx={{ marginRight: 1 }} />
            <h2 className="font-bold">{t("title.answers")}:</h2>
          </span>
        )}
        {hasTyping && message.status === "completed" ? (
          <TypingText text={message.content} role={message.role} />
        ) : (
          <Markdown markdownContent={message.content} />
        )}
      </section>
      {transformRefItems.length > 0 && (
        <section className="mb-2">
          <ReferenceSection referencesItem={transformRefItems} />
        </section>
      )}
    </React.Fragment>
  );
};
