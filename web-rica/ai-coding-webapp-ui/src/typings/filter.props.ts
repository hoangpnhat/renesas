import { BaseQueryParamsV2 } from "./component.props";

export interface AccessibleItem {
  id: string;
  name: string;
}

export interface FilterInput {
  filter_name: string;
  accessible_item: AccessibleItem[] | null;
}

export interface Condition {
  field: string;
  operator: string;
  value: string | string[];
}

export interface FilterGroup {
  group_operator: string;
  conditions: Condition[];
}

export interface FilterOutput {
  match_type: string;
  filter_groups: FilterGroup[];
  empty_result: string;
}

export interface FilterCondition {
  id: string;
  field: string;
  operator: string;
  value: string | string[];
}

export interface FilterGroupState {
  id: string;
  operator: "any" | "all";
  conditions: FilterCondition[];
}

export interface FieldOption {
  value: string;
  label: string;
  type: "list" | "date" | "numeric" | "text";
  accessible_items?: AccessibleItem[];
}

export interface OperatorOption {
  value: string;
  label: string;
}

export interface BaseFilterParams {
  status?: string;
  user_ids?: string[];
}

export interface BatchFilterParams extends BaseFilterParams {
  knowledge_ids?: string[];
  project_ids?: string[];
}

export interface DynamicFilterParams extends BaseQueryParamsV2 {
  advanced_filters?: FilterOutput;
  search_text?: string;
}
