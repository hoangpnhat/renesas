import { useEffect, useRef } from "react";
import type { ComboBoxItem } from "../../components/mainInput/types";

interface RicaDocsPageInputProps {
  item: ComboBoxItem;
  onSubmit: (params: {
    item: ComboBoxItem;
    query: string;
    label: string;
  }) => void;
  onCancel: () => void;
}

export function RicaDocsPageInput({
  item,
  onSubmit,
  onCancel,
}: RicaDocsPageInputProps) {
  const inputRef = useRef<HTMLInputElement>(null);

  useEffect(() => {
    inputRef.current?.focus();
  }, []);

  const submit = () => {
    const pages = inputRef.current?.value.trim() || "";
    const query = pages ? `${item.query}:${pages}` : item.query || "";
    const label = pages
      ? `${item.label || item.title} (p.${pages})`
      : item.label || item.title || "";
    onSubmit({ item, query, label });
  };

  return (
    <div className="flex items-center gap-1 px-2 py-1">
      <span className="mr-1 whitespace-nowrap text-xs opacity-70">
        {item.title}
      </span>
      <input
        ref={inputRef}
        type="text"
        className="flex-1 rounded border border-solid border-zinc-600 bg-transparent px-2 py-1 text-xs outline-none focus:border-blue-500"
        placeholder="pages: 1-5, 8 (leave empty to use AI search)"
        onClick={(e) => e.stopPropagation()}
        onKeyDown={(e) => {
          if (e.key === "Enter") {
            e.preventDefault();
            submit();
          } else if (e.key === "Escape") {
            onCancel();
          }
        }}
      />
      <button
        className="rounded bg-zinc-700 px-2 py-1 text-xs hover:bg-zinc-600"
        onClick={submit}
      >
        ⏎
      </button>
    </div>
  );
}
