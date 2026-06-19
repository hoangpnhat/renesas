/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { MenuItemType, SelectionItem } from "../../../typings/component.props";
import EditIcon from "@mui/icons-material/Edit";
import DeleteIcon from "@mui/icons-material/Delete";
import ContentCopyOutlinedIcon from "@mui/icons-material/ContentCopyOutlined";
import EmojiObjectsOutlinedIcon from "@mui/icons-material/EmojiObjectsOutlined";

export const checkBoxTypeDoc: SelectionItem[] = [
  { name: "Code Base", value: "code_base" },
  {
    name: "Document",
    value: "document"
  }
];
export const menuItems: MenuItemType[] = [
  {
    text: "menuItem.rename",
    icon: <EditIcon />,
    key: "rename"
  },
  {
    text: "menuItem.delete",
    key: "delete",
    icon: <DeleteIcon />
  }
];

export const menuContextChat: MenuItemType[] = [
  {
    text: "text.explain",
    icon: <EmojiObjectsOutlinedIcon />,
    key: "explain"
  },
  {
    text: "text.copy",
    icon: <ContentCopyOutlinedIcon />,
    key: "copy"
  }
  // {
  //   text: "Improve",
  //   key: "improve",
  //   icon: <ChatIcon />,
  // },
];

// export const selectionArrConfigs: SelectionOptionConfigs[] = [
//   {
//     isMultiple: false,
//     keyString: "vector_search",
//     selectionArr: [
//       // {
//       //   name: RULE_NAME_DISPLAY.global_rules_vector_search,
//       //   value: RULE_NAME_VALUE.global_rules_vector_search,
//       // },
//       {
//         name: RULE_NAME_DISPLAY.ringi_items_vector_search,
//         value: RULE_NAME_VALUE.ringi_items_vector_search,
//       },
//     ],
//     titleString: "title.rules",
//   },
//   // {
//   //   keyString: "categories",
//   //   isMultiple: true,
//   //   selectionArr: [
//   //     {
//   //       value: "RGR-AA01",
//   //       name: "RGR-AA01",
//   //     },
//   //     {
//   //       value: "RGR-AC01",
//   //       name: "RGR-AC01",
//   //     },
//   //     {
//   //       value: "RGR-AC02",
//   //       name: "RGR-AC02",
//   //     },
//   //     {
//   //       value: "RGR-AD01",
//   //       name: "RGR-AD01",
//   //     },
//   //     {
//   //       value: "RGR-BA01",
//   //       name: "RGR-BA01",
//   //     },
//   //     {
//   //       value: "RGR-BA02",
//   //       name: "RGR-BA02",
//   //     },
//   //     {
//   //       value: "RGR-DA01",
//   //       name: "RGR-DA01",
//   //     },
//   //     {
//   //       value: "RGR-DA02",
//   //       name: "RGR-DA02",
//   //     },
//   //     {
//   //       value: "RGR-DC01",
//   //       name: "RGR-DC01",
//   //     },
//   //     {
//   //       value: "RGR-DC02",
//   //       name: "RGR-DC02",
//   //     },
//   //     {
//   //       value: "RGR-EA01",
//   //       name: "RGR-EA01",
//   //     },
//   //     {
//   //       value: "RGR-FA01",
//   //       name: "RGR-FA01",
//   //     },
//   //     {
//   //       value: "RGR-FA02",
//   //       name: "RGR-FA02",
//   //     },
//   //     {
//   //       value: "RGR-FA03",
//   //       name: "RGR-FA03",
//   //     },
//   //     {
//   //       value: "RGR-GA01",
//   //       name: "RGR-GA01",
//   //     },
//   //     {
//   //       value: "RGR-HA01",
//   //       name: "RGR-HA01",
//   //     },
//   //     {
//   //       value: "RGR-HA02",
//   //       name: "RGR-HA02",
//   //     },
//   //     {
//   //       value: "RGR-JA01",
//   //       name: "RGR-JA01",
//   //     },
//   //     {
//   //       value: "RGR-LA01",
//   //       name: "RGR-LA01",
//   //     },
//   //     {
//   //       value: "RGR-MA01",
//   //       name: "RGR-MA01",
//   //     },
//   //     {
//   //       value: "RGR-NA01",
//   //       name: "RGR-NA01",
//   //     },
//   //     {
//   //       value: "RGR-NA02",
//   //       name: "RGR-NA02",
//   //     },
//   //     {
//   //       value: "RGR-QA01",
//   //       name: "RGR-QA01",
//   //     },
//   //     {
//   //       value: "RGR-QA02",
//   //       name: "RGR-QA02",
//   //     },
//   //     {
//   //       value: "RGR-RA01",
//   //       name: "RGR-RA01",
//   //     },
//   //     {
//   //       value: "RGR-BA001",
//   //       name: "RGR-BA001",
//   //     },
//   //     {
//   //       value: "RGR-BA002",
//   //       name: "RGR-BA002",
//   //     },
//   //     {
//   //       value: "RGR-DD001",
//   //       name: "RGR-DD001",
//   //     },
//   //     {
//   //       value: "RGR-FA001",
//   //       name: "RGR-FA001",
//   //     },
//   //     {
//   //       value: "RGR-FA002",
//   //       name: "RGR-FA002",
//   //     },
//   //     {
//   //       value: "RGR-HA001",
//   //       name: "RGR-HA001",
//   //     },
//   //     {
//   //       value: "RGR-HA002",
//   //       name: "RGR-HA002",
//   //     },
//   //     {
//   //       value: "RGR-HA003",
//   //       name: "RGR-HA003",
//   //     },
//   //     {
//   //       value: "RGR-LA001",
//   //       name: "RGR-LA001",
//   //     },
//   //     {
//   //       value: "RGR-LA002",
//   //       name: "RGR-LA002",
//   //     },
//   //     {
//   //       value: "RGR-MA001",
//   //       name: "RGR-MA001",
//   //     },
//   //     {
//   //       value: "RGR-NA001",
//   //       name: "RGR-NA001",
//   //     },
//   //     {
//   //       value: "RGR-PA001",
//   //       name: "RGR-PA001",
//   //     },
//   //   ],
//   //   titleString: "Categories",
//   // },
// ];
