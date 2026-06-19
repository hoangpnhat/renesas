/*
 * Copyright © 2024 Renesas Electronics Corporation. All Rights Reserved.
 */

import { BaseInterface, BaseObject } from ".";
import { MessageStatus, MessageType } from "./component.props";
import { Language } from "./common.props.ts";

export interface Message {
  content: string;
  role: string;
  dialog_id: string;
  retrieval_place: string[];
  is_good_response?: 1 | -1 | 0;
  language: Language;
  token?: string;
}

export interface MessageContentUpdate extends Message, BaseInterface {}

export interface MessageContentUpdateResponseBody {
  message_content: MessageType;
  dialog: DialogResult;
}

export interface CreateDialogRequestBody {
  title: string;
}

export interface Status {
  state: string;
  message: string;
}

export interface UserSettingsRequestBody {
  preferred_language: Language;
}

export interface UserSettingsResponseBody {
  preferred_language: string;
  mode: string;
}

export interface UserMeResponseBody {
  created: string,
  modified: string,
  full_name: string,
  email: string,
  settings?: UserSettingsResponseBody,
  id: string
}


export interface AllDialogResponseBody {
  status: Status;
  results: DialogResult[];
}

export interface DialogResult extends BaseInterface {
  user_id: string;
  start_time: Date;
  last_modified: Date;
  content_ids: string[];
  title: string;
}

export interface RetrievedDocs extends BaseInterface, BaseObject {
  content: string;
  doc_uri: string;
  name: string;
  file_name: string;
}

export interface ContentDialogResults {
  content: string;
  role: string;
  relate_to: string;
  dialog_id: string;
  timestamp: Date;
  id: string;
  has_error: boolean;
  status: MessageStatus;
  is_favorite: boolean;
  retrieved_docs?: RetrievedDocs[];
  vector_search: string;
  categories: string[];
}

export interface ContentsDialogResponseBody
  extends BaseResponseBody<Partial<ContentDialogResults>[]> {}

export interface BaseResponseBody<T> {
  metadata: MetaData;
  status: Status;
  results: T;
}

export interface BaseListResponse<T> {
  data: T[];
  pagination: {
    page: number;
    limit: number;
    total: number;
  };
}

export interface DeleteDialogResponseBody {
  status: Status;
  id: string;
}

export interface MetaData {
  all_records: number;
  limit: number;
  page: number;
  total_page: number;
}

export interface UpdatedDialogResponseBody
  extends BaseResponseBody<DialogResult> {}

export interface SavedChatContentResponseBody
  extends BaseResponseBody<SavedChatContent | SavedChatContent[]> {}

export interface SavedChatContent {
  user_prompt: string;
  bot_response: string;
  categories: string[];
  vector_search: string;
  references: RetrievedDocs[];
  content_id: string;
  dialog_id: string;
  is_disabled: boolean;
  dialog_title: string;
  user_id: string;
  id: string;
}

export interface GraphMe {
  "@odata.context": string;
  businessPhones: string[];
  displayName: string;
  givenName: string;
  jobTitle: string;
  mail: string;
  mobilePhone: null;
  officeLocation: string;
  preferredLanguage: null;
  surname: string;
  userPrincipalName: string;
  id: string;
}

export interface GetBaseResponseBody<T> {
  page: number;
  entries: number;
  total: number;
  data: T[];
}

export interface GroupMember {
  user_id: string;
  role: string;
  full_name?: string;
  joined_at?: string | null;
}

export interface GroupResource {
  resource_id: string;
  resource_type: string;
  resource_name: string;
  last_modified: string;
}

export interface Group {
  is_owner?: boolean;
  is_admin?: boolean;
  id: string;
  owner_id: string;
  name: string;
  description?: string;
  visibility: string;
  members: GroupMember[];
  resources?: GroupResource[];
  is_deleted?: boolean;
  is_archived?: boolean;
  created_at: string;
  last_modified?: string;
  owner_name?: string;
  can_perform_critical_actions?: boolean;
}

export interface GroupsListResponse extends BaseListResponse<Group> {}

export interface CreateGroupRequest {
  name: string;
  description?: string;
  visibility: string;
  members: GroupMember[];
}

export interface NotificationMetadata {
  kb_name?: string;
  group_name?: string;
  inviter_name?: string;
  action_completed?: boolean;
}

export interface Notification {
  id: string;
  title: string;
  message: string;
  status: string;
  notification_type: string;
  resource_type: string;
  resource_id: string;
  actor_id: string;
  metadata: NotificationMetadata;
  action_url: string;
  created_at: string;
}

export interface NotificationsListResponse extends BaseListResponse<Notification> {}
