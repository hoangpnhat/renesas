import { ReferenceDetailsProps } from "../../../typings/component.props";
import Link from "@mui/material/Link";
import Paper from "@mui/material/Paper";
import Tooltip from "@mui/material/Tooltip";
import Typography from "@mui/material/Typography";
import { useIconGetter } from "../../../hooks/useIconGetter";
import ReactMarkdown from "react-markdown";
import { useMemo } from "react";
import { environment } from "../../../environments/environment.dev.ts";
import { END_POINT } from "../../../requests/endpoint.ts";
import { createQueryParams } from "../../../utils/utilities.ts";

export const ReferenceDetails = ({ refItem }: ReferenceDetailsProps) => {
  const { getIconFileType } = useIconGetter({
    fontSize: "large",
    className: "m-2",
  });
  const newLink = useMemo(() => {
    return createQueryParams(`${environment.baseURL}${END_POINT.download}`, {
      path_file: refItem.doc_uri,
    });
  }, [refItem]);
  return (
    <div className="w-full h-full">
      <section className="overflow-y-auto max-h-[77%] relative">
        <ReactMarkdown>{refItem.content}</ReactMarkdown>
      </section>

      <div className="my-2 w-full absolute bottom-0 mt-auto">
        <Typography
          component="h2"
          fontWeight="700"
          fontSize="large"
          className="font-bold"
        >
          Documents:
        </Typography>
        <Tooltip title="Click to open file">
          <Paper className="flex w-auto mt-2 items-center">
            {getIconFileType(refItem.doc_uri)}
            <Link href={newLink} underline="hover">
              Click to download
            </Link>
          </Paper>
        </Tooltip>
      </div>
    </div>
  );
};
