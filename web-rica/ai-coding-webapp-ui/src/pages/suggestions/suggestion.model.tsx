import {
  CommonChipModel,
  RadioGroupSelectItem,
} from "../../typings/component.props";
import { RAG_NAME_DISPLAY, RAG_NAME_VALUE } from "../../constants/common";

export const isMultipleStyle = {
  height: "auto",
  "& .MuiChip-label": {
    display: "block",
    whiteSpace: "normal",
    textWrap: "pretty",
  },
};
export const globalRuleSamplePrompts: CommonChipModel[] = [
  {
    label:
      "Which IT concern I should be aware of when traveling to a new country?",
    sx: isMultipleStyle,
    realText:
      "Which IT concern I should be aware of when traveling to a new country?",
  },
  {
    label: "When I use Renesas logo, what should I concern?",
    sx: isMultipleStyle,
    realText: "When I use Renesas logo, what should I concern?",
  },
  {
    label: "What is Renesas Electronics Group CSR Charter?",
    sx: isMultipleStyle,
    realText: "What is Renesas Electronics Group CSR Charter?",
  },
];

export const ringiSamplePrompts: CommonChipModel[] = [
  {
    label:
      "I would like to inquire whether the Ringi approval is needed for Expatriate House Rental agreement?",
    sx: isMultipleStyle,
    realText:
      "I would like to inquire whether the Ringi approval is needed for Expatriate House Rental agreement?",
  },
  {
    label: "Ringi Approval for risk assessment",
    sx: isMultipleStyle,
    realText:
      "We are workin on carrying out a psychological risk assessment (statutory survey project for all employees) throughout Germany this year. The cost are to be spread across all German entities. Can you tell me whether we need ad RINGI approval for this or whether a PO is sufficient?",
  },
  {
    label: "What should include in my security report?",
    sx: isMultipleStyle,
    realText: "What should include in my security report?",
  },
];

export const promptSampleSelections: { [key: string]: CommonChipModel[] } = {
  [RAG_NAME_VALUE.code_base]: globalRuleSamplePrompts,
  [RAG_NAME_VALUE.document]: ringiSamplePrompts,
};

export const selectRules: RadioGroupSelectItem[] = [
  // {
  //   name: RULE_NAME_DISPLAY.global_rules_vector_search,
  //   value: RULE_NAME_VALUE.global_rules_vector_search,
  // },
  {
    name: RAG_NAME_DISPLAY.code_base,
    value: RAG_NAME_VALUE.code_base,
  },
  {
    name: RAG_NAME_DISPLAY.document,
    value: RAG_NAME_VALUE.document,
  },
];
