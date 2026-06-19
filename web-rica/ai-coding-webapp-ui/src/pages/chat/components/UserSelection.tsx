// import React, { useEffect, useState } from "react";
// import {
//   SelectionItem,
//   UserSelectionProps,
// } from "../../../typings/component.props";
// import { CommonSelectionComponent } from "../../../components/Selection";
// import { UserSelectionState } from "../typings/state";
// import { Typography } from "@mui/material";
// import { useSelector } from "react-redux";
// import { RootState } from "../../../store";
//
// export const UserSelection = ({
//   selectionArrayConfigs,
//   onChangeSelectOption,
//   title,
// }: UserSelectionProps) => {
//   const vectorSearch = useSelector(
//     (state: RootState) => state.input.vector_search,
//   );
//   const [selectedItems, setSelectedItems] = useState<UserSelectionState>({
//     categories: [],
//     vector_search: vectorSearch[0],
//   });
//
//   useEffect(() => {
//     onChangeSelectOption(selectedItems);
//   }, [selectedItems]);
//   const onUserOptionChange = (
//     newValue: SelectionItem[] | SelectionItem | null,
//     keyString: string,
//   ) => {
//     setSelectedItems({
//       ...selectedItems,
//       [keyString]: newValue,
//     });
//   };
//   return (
//     <React.Fragment>
//       <div className="flex flex-col w-full my-2 justify-between py-2 px-2">
//         <Typography>
//           <span className="font-bold text-center flex items-center ml-2 mb-4 text-base">
//             {title}
//           </span>
//         </Typography>
//         {selectionArrayConfigs?.map((selectionItem, index) => {
//           return (
//             <CommonSelectionComponent
//               selectedValue={
//                 selectedItems[
//                   selectionItem.keyString as keyof UserSelectionState
//                 ]
//               }
//               isDisable={
//                 selectionItem.keyString === "categories" &&
//                 (!selectedItems.vector_search.value ||
//                   [
//                     "ringi_items_vector_search",
//                     "ringi_basic_vector_search",
//                   ].includes(selectedItems.vector_search.value))
//               }
//               key={index}
//               onChangeSelection={onUserOptionChange}
//               {...selectionItem}
//             />
//           );
//         })}
//       </div>
//     </React.Fragment>
//   );
// };
