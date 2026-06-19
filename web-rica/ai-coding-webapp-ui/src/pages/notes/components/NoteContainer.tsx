import { Fragment } from "react/jsx-runtime"
import Grid from '@mui/material/Grid2';
import { NoteCard } from "./NoteCard";

export const NoteContainer = () => {
  return <Fragment><Grid size={{sm: 12}} container>
    <Grid size={{xs: 12, sm: 6, md: 4, lg: 3, xl:2}}>
      <NoteCard/>
      </Grid>
    </Grid></Fragment>
}