import { useEffect } from "react";
import { useLocation } from "react-router-dom";
import { useDispatch } from "react-redux";
import { setTitleProject } from "../utils/utilities";
import { useTranslation } from "react-i18next";
import camelCase from "lodash/camelCase";
import { closeDrawer } from "../store/reducers/drawer.slice.ts";

export const useChangePath = () => {
  const location = useLocation();
  const { t } = useTranslation();
  const dispatch = useDispatch();

  useEffect(() => {
    // Close any open drawer when the route changes
    dispatch(closeDrawer());

    if (location.pathname.endsWith("new")) {
      document.title = setTitleProject("New Chat");
    } else {
      document.title = setTitleProject(
        t(
          "title.layout." +
            camelCase(location.pathname?.split("/")?.at(2) || "home"),
        ),
      );
    }
  }, [location, t, dispatch]);
};
