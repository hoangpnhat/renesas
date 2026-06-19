import Typography from "@mui/material/Typography";
import Container from "@mui/material/Container";
import { CommonTable } from "../../components/CommonTable";
import { displayColumns, itemRows, listColumnNames } from "./about.models";

const AboutUsPage = () => {
  return (
    <div className="min-h-screen bg-gradient-to-b from-blue-100 to-white py-12 overflow-auto">
      <Container maxWidth="lg">
        <Typography
          variant="h2"
          className="text-center mb-8 text-blue-800 font-bold"
        >
          Global Rules
        </Typography>

        {/* Adding a table for displaying rules */}
        <Typography variant="h4" className="mb-4 text-blue-700">
          Company Rules
        </Typography>

        <CommonTable
          listColumnNames={listColumnNames}
          itemsRows={itemRows}
          displayColumns={displayColumns}
        />
      </Container>
    </div>
  );
};

export default AboutUsPage;
