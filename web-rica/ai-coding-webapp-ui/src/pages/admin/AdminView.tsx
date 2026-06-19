/*
 * Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
 */
import React from "react";
import { Navigate, Outlet } from "react-router-dom";
import { useSelector } from "react-redux";
import { RootState } from "../../store";
import { checkIfKeyInArr } from "../../utils/utilities.ts";
import { APP_ROLE } from "../../constants/common.ts";

const AdminView = () => {
  const isAdmin = useSelector((state: RootState) =>
    checkIfKeyInArr(state.user.roles, [APP_ROLE.SUPER_ADMIN, APP_ROLE.ADMIN]),
  );

  return (
    <React.Fragment>
      {isAdmin ? <Outlet /> : <Navigate to={"/"} />}
    </React.Fragment>
  );
};

export default AdminView;
