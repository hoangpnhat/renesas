import { createBrowserRouter } from "react-router-dom";

import React from "react";
import { MainLayoutComponent } from "../layout/MainLayout";
import ModelMainScreen from "../pages/admin/model-management/ModelMainScreen.tsx";

// const MainViewChatComponent = React.lazy(
//   () => import("../pages/chat/MainViewChat"),
// );
const AdminView = React.lazy(() => import("../pages/admin/AdminView"));
// const ChatBotLandingPage = React.lazy(
//   () => import("../pages/chat/ChatBotLandingPage.tsx"),
// );

const FileManagementView = React.lazy(
  () => import("../pages/files-management/FileManagementMainView.tsx"),
);

const KnowledgeBaseManagementMainView = React.lazy(
  () =>
    import(
      "../pages/knowledge-base-management/KnowledgeBaseManagementMainView.tsx"
    ),
);

const GroupListView = React.lazy(
  () => import("../pages/groups/GroupListView.tsx"),
);

const GroupDetailPage = React.lazy(
  () => import("../pages/groups/GroupDetailPage.tsx"),
);

const RuleMainView = React.lazy(
  () => import("../pages/customize/RuleMainView.tsx"),
);

const PromptMainView = React.lazy(
  () => import("../pages/customize/PromptMainView.tsx"),
);

// const ChatComponent = React.lazy(() => import("../pages/chat/Chat"));
export const router = createBrowserRouter([
  {
    path: "/",
    element: <MainLayoutComponent />,
    children: [
      {
        index: true,
        element: <KnowledgeBaseManagementMainView />,
      },
      {
        path: "admin",
        element: <AdminView />,
        children: [
          {
            path: "model-management",
            element: <ModelMainScreen />,
          },
        ],
      },
      // {
      //   path: "chat",
      //   element: <MainViewChatComponent />,
      //   children: [
      //     {
      //       path: ":dialog_id",
      //       element: <ChatComponent />,
      //     },
      //   ],
      // },
      {
        path: "management/file",
        element: <FileManagementView />,
      },
      {
        path: "management/knowledgeBase",
        element: <KnowledgeBaseManagementMainView />,
      },
      {
        path: "management/groups",
        element: <GroupListView />,
      },
      {
        path: "management/groups/:groupId",
        element: <GroupDetailPage />,
      },
      {
        path: "customize/rule",
        element: <RuleMainView />,
      },
      {
        path: "customize/prompt",
        element: <PromptMainView />,
      },
    ],
  },
]);
