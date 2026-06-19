import React, {
  PropsWithChildren,
  useCallback,
  useEffect,
  useMemo,
  useRef,
  useState,
} from "react";
import {
  HistoryChatItem,
  HistoryChatProps,
  HistoryChatTransform,
} from "../../../typings/component.props";
import Box from "@mui/material/Box";
import IconButton from "@mui/material/IconButton";
import List from "@mui/material/List";
import ListItemButton from "@mui/material/ListItemButton";
import ListItemIcon from "@mui/material/ListItemIcon";
import ListItemText from "@mui/material/ListItemText";
import Skeleton from "@mui/material/Skeleton";
import Tooltip from "@mui/material/Tooltip";
import Typography from "@mui/material/Typography";
import { useNavigate, useParams } from "react-router-dom";
import MoreVertIcon from "@mui/icons-material/MoreVert";
import { CommonMenuWithIcon } from "../../../components/CommonMenuWithIcon";
import { menuItems } from "./chat.models";
import { useMenuRef } from "../../../hooks/useMenuRef";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "../../../store";
import { updateCommon } from "../../../store/reducers/common.slice";
import ExpandLess from "@mui/icons-material/ExpandLess";
import ExpandMore from "@mui/icons-material/ExpandMore";
import { CommonCollapse } from "../../../components/CommonCollapse";
import { useTransformHistoryChat } from "../../../hooks/useTransformHistoryChat";
import { useDialogActions } from "../hooks/useDialogActions";
import Checkbox from "@mui/material/Checkbox";
import { useTranslation } from "react-i18next";
import {
  transformArrayToObjectBoolean,
  transformObjectBooleanToArray,
} from "../../../utils/utilities.ts";
import { CommonInterface } from "../../../components/CommonFormGroup.tsx";
import DeleteOutlineOutlinedIcon from "@mui/icons-material/DeleteOutlineOutlined";
import ClearOutlinedIcon from "@mui/icons-material/ClearOutlined";
import { MultipleSelectIcon } from "../../../components/icons/MultipleSelectIcon.tsx";
import CheckBoxOutlinedIcon from "@mui/icons-material/CheckBoxOutlined";
import cloneDeep from "lodash/cloneDeep";
import { updateDialogConfig } from "../../../store/reducers/dialogComponent.slice.ts";
import { deleteMultipleDialogs } from "../../requests/chat-request.ts";
import { removeDialogItems } from "../../../store/reducers/dialog.slice.ts";
// Todo:
// [ ]: Add lazy loading for this history chat
const HistoryChat = ({ history }: PropsWithChildren<HistoryChatProps>) => {
  const [transformHistory, setTransformHistory] =
    useState<HistoryChatTransform>({});
  const { handleClick, handleClose, anchorEl } = useMenuRef("anchorEl");
  const [isSelectMultiple, setIsSelectMultiple] = useState(false);
  const { dialog_id } = useParams();
  const [checkedValues, setCheckedValues] = useState<CommonInterface>({});
  const commonContext = useSelector((state: RootState) => state.common);
  const dispatch = useDispatch();
  const ref = useRef(null);
  const [selectedHistoryItem, setSelectedHistoryItem] =
    useState<HistoryChatItem>();
  const { actionRef } = useDialogActions();
  const { t } = useTranslation();

  // const [selectedIndex, setSelectedIndex] = useState("");
  const navigate = useNavigate();
  const handleSelectedItem = (
    event: React.MouseEvent<HTMLDivElement, MouseEvent>,
    id: string,
  ) => {
    event.preventDefault();
    // setSelectedIndex(keyName);
    if (!isSelectMultiple) {
      dispatch(updateCommon({ isSelectedSidebar: id }));
      navigate(`/chat/${id}`);
    } else {
      handleChange(id);
    }
  };
  useEffect(() => {
    if (history.length > 0) {
      setCheckedValues(
        transformArrayToObjectBoolean<HistoryChatItem>(history, "id"),
      );
    }
  }, [history]);
  const { historyChatTransform } = useTransformHistoryChat(
    history,
    setTransformHistory,
  );
  const isNotSelectAll = useMemo(() => {
    return Object.keys(checkedValues).some(
      (key: string) => !checkedValues[key],
    );
  }, [checkedValues]);
  const isSelected = useMemo(
    () => Object.keys(checkedValues).some((key: string) => checkedValues[key]),
    [checkedValues],
  );
  const handleExpandClick = (ev: React.MouseEvent, itemStr: string) => {
    ev.preventDefault();
    const targetTransform = transformHistory[itemStr];

    setTransformHistory({
      ...transformHistory,
      [itemStr]: {
        ...targetTransform,
        isCollapse: !targetTransform.isCollapse,
      },
    });
  };
  const handleOnclickMultipleSelect = (
    event: React.MouseEvent<HTMLButtonElement>,
  ) => {
    // event.preventDefault();
    setIsSelectMultiple(!isSelectMultiple);
    handleSelectAll(event, false);
  };
  const handleChange = (name: string) => {
    setCheckedValues({
      ...checkedValues,
      [name]: !checkedValues[name],
    });
  };
  const handleSelectAll = (
    event: React.MouseEvent,
    newValue: boolean = true,
  ) => {
    event.preventDefault();
    const tempObj = cloneDeep(checkedValues);
    Object.keys(tempObj).forEach((key) => {
      tempObj[key] = newValue;
    });
    setCheckedValues(tempObj);
  };

  const handleDeleteMultiple = useCallback(
    (event: React.MouseEvent) => {
      event.preventDefault();
      dispatch(
        updateDialogConfig({
          isOpen: true,
          title: t("title.deleteDialog"),
          fullWidth: false,
          hasConfirmButton: true,
          hasCancelButton: true,
          hasAction: true,
          children: <span>{t("text.confirmDeleteDialog")}</span>,
          onSubmit: async () => {
            const deleteDialogs = transformObjectBooleanToArray(checkedValues);
            console.log("=>(HistoryChat.tsx:155) checkedValues", checkedValues);
            console.log("=>(HistoryChat.tsx:155) deleteDialogs", deleteDialogs);

            await deleteMultipleDialogs(deleteDialogs);
            dispatch(removeDialogItems(deleteDialogs));
            if (deleteDialogs.includes(dialog_id as string)) {
              navigate("/chat/undefined");
            }
          },
        }),
      );
    },
    [checkedValues],
  );

  return (
    <React.Fragment>
      <div className="flex justify between w-full">
        {history.length > 0 && (
          <Tooltip
            arrow={true}
            placement="top"
            title={
              isSelectMultiple
                ? t("labelButton.cancel")
                : t("labelButton.selectMultiple")
            }
          >
            <IconButton onClick={handleOnclickMultipleSelect} className="mr-2">
              {!isSelectMultiple ? (
                <MultipleSelectIcon fontSize="small" />
              ) : (
                <ClearOutlinedIcon fontSize="small" />
              )}
            </IconButton>
          </Tooltip>
        )}
        {isSelectMultiple && (
          <section className="flex w-full justify-end">
            {isNotSelectAll && (
              <Tooltip title="Select all" arrow>
                <IconButton onClick={handleSelectAll}>
                  <CheckBoxOutlinedIcon />
                </IconButton>
              </Tooltip>
            )}
            {isSelected && (
              <Tooltip title="Delete Selected" arrow>
                <IconButton onClick={handleDeleteMultiple}>
                  <DeleteOutlineOutlinedIcon />
                </IconButton>
              </Tooltip>
            )}
          </section>
        )}
      </div>
      <List disablePadding>
        {(history.length > 0 && Object.keys(historyChatTransform).length > 0) ||
        !commonContext.isLoading ? (
          Object.keys(historyChatTransform).map((historyKey, hisId) => {
            return (
              <div className="w-full mt-2" key={hisId}>
                <span className="w-full font-bold">
                  <ListItemButton
                    className="w-full mt-2"
                    dense
                    sx={{
                      py: 0,
                      px: 1,
                      minHeight: 28, // Even smaller height
                      "& .MuiTypography-root": {
                        fontSize: "0.875rem", // Smaller font size
                      },
                    }}
                    key={hisId}
                    onClick={(ev) => handleExpandClick(ev, historyKey)}
                  >
                    <ListItemText
                      sx={{
                        "&>span.MuiListItemText-primary": {
                          fontWeight: 700,
                        },
                      }}
                      primary={historyKey}
                    />
                    {transformHistory?.[historyKey]?.isCollapse ? (
                      <ExpandMore />
                    ) : (
                      <ExpandLess />
                    )}
                  </ListItemButton>
                  <CommonCollapse
                    open={!transformHistory?.[historyKey]?.isCollapse}
                  >
                    <List dense={false} className="font-medium">
                      {historyChatTransform[historyKey].history?.map(
                        (historyValue, index) => {
                          return (
                            <Tooltip
                              placement="right"
                              arrow
                              title={historyValue.title}
                              key={index}
                            >
                              <span className="flex group">
                                <ListItemButton
                                  disabled={commonContext.isThinkingChatBot}
                                  className="group"
                                  key={index}
                                  sx={{
                                    paddingBottom: 0,
                                    paddingTop: 0,
                                  }}
                                  selected={
                                    commonContext.isSelectedSidebar ===
                                    historyValue.id
                                  }
                                  onClick={(event) =>
                                    handleSelectedItem(event, historyValue.id)
                                  }
                                >
                                  {isSelectMultiple && (
                                    <ListItemIcon>
                                      <Checkbox
                                        disableRipple
                                        name={historyValue.id}
                                        checked={checkedValues[historyValue.id]}
                                        // name={item.value}
                                      />
                                    </ListItemIcon>
                                  )}
                                  <ListItemText
                                    ref={ref}
                                    sx={{
                                      whiteSpace: "nowrap",
                                      overflow: "hidden",
                                      textOverflow: "ellipsis",
                                      "&>span.MuiListItemText-primary": {
                                        fontSize: "0.875rem",
                                      },
                                    }}
                                    primary={historyValue.title}
                                  />
                                  <IconButton
                                    className="invisible group-hover:visible ml-4"
                                    onClick={(event) => {
                                      event.stopPropagation();
                                      handleClick(event);
                                      setSelectedHistoryItem(historyValue);
                                    }}
                                  >
                                    <MoreVertIcon />
                                  </IconButton>
                                </ListItemButton>
                              </span>
                            </Tooltip>
                          );
                        },
                      )}
                    </List>
                  </CommonCollapse>
                </span>
              </div>
            );
          })
        ) : (
          <SkeletonHistoryChat />
        )}
      </List>
      <CommonMenuWithIcon
        menuItems={menuItems}
        handleClose={handleClose}
        selectedItem={selectedHistoryItem}
        anchorEl={anchorEl as HTMLElement}
        actionRef={actionRef}
        type="anchorEl"
      />
    </React.Fragment>
  );
};

export const SkeletonHistoryChat = () => (
  <Box sx={{ width: "100%", backgroundColor: "inherit" }}>
    <Typography variant="h6" component="div" sx={{ p: 2, bgcolor: "primary" }}>
      <Skeleton width="100%" />
    </Typography>
    {[...Array(10)].map((_, index) => (
      <Box key={index} sx={{ p: 2 }}>
        <Skeleton width="80%" />
      </Box>
    ))}
  </Box>
);
export default HistoryChat;
