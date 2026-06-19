import { Fragment } from "react/jsx-runtime";

export const NoteCard = () => {
  return (
    <Fragment>
      <div className="w-full h-64 flex flex-col justify-between items-start bg-surface-base rounded-lg border border-border-default mb-6 py-5 px-4">
        <div>
          <h4 className="text-text-primary font-bold mb-3">
            13 things to work on
          </h4>
          <p className="text-text-secondary text-sm">
            Probabo, inquit, sic agam, ut labore et voluptatem sequi nesciunt,
            neque porro quisquam est, quid malum, sensu iudicari
          </p>
        </div>
        <div className="w-full flex flex-col items-start">
          <div
            className="mb-3 border border-border-light rounded-full px-3 py-1 text-text-secondary text-xs flex items-center"
            aria-label="Due on"
            role="contentinfo"
          >
            <svg
              xmlns="http://www.w3.org/2000/svg"
              className="icon icon-tabler icon-tabler-alarm"
              width="16"
              height="16"
              viewBox="0 0 24 24"
              strokeWidth="1.5"
              stroke="currentColor"
              fill="none"
              strokeLinecap="round"
              strokeLinejoin="round"
            >
              <path stroke="none" d="M0 0h24v24H0z"></path>
              <circle cx="12" cy="13" r="7"></circle>
              <polyline points="12 10 12 13 14 13"></polyline>
              <line x1="7" y1="4" x2="4.25" y2="6"></line>
              <line x1="17" y1="4" x2="19.75" y2="6"></line>
            </svg>
            <p className="ml-2">7 Sept, 23:00</p>
          </div>
          <div className="flex items-center justify-between text-text-muted w-full">
            <p className="text-sm">March 28, 2020</p>
            <button
              className="w-8 h-8 rounded-full bg-primary-main text-text-primary flex items-center justify-center focus:outline-none focus:ring-2 focus:ring-offset-2 ring-offset-surface-base focus:ring-primary-light"
              aria-label="edit note"
              role="button"
            >
              <svg
                xmlns="http://www.w3.org/2000/svg"
                className="icon icon-tabler icon-tabler-pencil"
                width="20"
                height="20"
                viewBox="0 0 24 24"
                strokeWidth="1.5"
                stroke="currentColor"
                fill="none"
                strokeLinecap="round"
                strokeLinejoin="round"
              >
                <path stroke="none" d="M0 0h24v24H0z"></path>
                <path d="M4 20h4l10.5 -10.5a1.5 1.5 0 0 0 -4 -4l-10.5 10.5v4"></path>
                <line x1="13.5" y1="6.5" x2="17.5" y2="10.5"></line>
              </svg>
            </button>
          </div>
        </div>
      </div>
    </Fragment>
  );
};
