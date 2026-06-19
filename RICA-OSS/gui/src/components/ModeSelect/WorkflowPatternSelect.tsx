import {
  ArrowsRightLeftIcon,
  ArrowsUpDownIcon,
  ArrowPathIcon,
  CheckIcon,
  ChevronDownIcon,
  CpuChipIcon,
  UserGroupIcon,
} from "@heroicons/react/24/outline";
import { WorkflowPattern } from "core";
import { useCallback } from "react";
import { useAppDispatch, useAppSelector } from "../../redux/hooks";
import { setWorkflowPattern } from "../../redux/slices/sessionSlice";
import { useMainEditor } from "../mainInput/TipTapEditor";
import {
  Listbox,
  ListboxButton,
  ListboxOption,
  ListboxOptions,
} from "../ui/Listbox";

const PATTERN_CONFIG: Record<
  WorkflowPattern,
  { label: string; icon: React.ComponentType<{ className?: string }> }
> = {
  auto: { label: "Auto", icon: CpuChipIcon },
  sequential: { label: "Sequential", icon: ArrowsUpDownIcon },
  parallel: { label: "Parallel", icon: ArrowsRightLeftIcon },
  iterative: { label: "Iterative", icon: ArrowPathIcon },
  collaborative: { label: "Collaborative", icon: UserGroupIcon },
};

const PATTERNS: WorkflowPattern[] = [
  "auto",
  "sequential",
  "parallel",
  "iterative",
  "collaborative",
];

export function WorkflowPatternSelect() {
  const dispatch = useAppDispatch();
  const pattern = useAppSelector(
    (store) => store.session.workflowPattern ?? "auto",
  );
  const { mainEditor } = useMainEditor();

  const selectPattern = useCallback(
    (newPattern: WorkflowPattern) => {
      if (newPattern === pattern) return;
      dispatch(setWorkflowPattern(newPattern));
      mainEditor?.commands.focus();
    },
    [pattern, mainEditor, dispatch],
  );

  const current = PATTERN_CONFIG[pattern];
  const Icon = current.icon;

  return (
    <Listbox value={pattern} onChange={selectPattern}>
      <div className="relative">
        <ListboxButton
          data-testid="workflow-pattern-select-button"
          className="xs:px-2 text-description bg-lightgray/20 gap-1 rounded-full border-none px-1.5 py-0.5 transition-colors duration-200 hover:brightness-110"
        >
          <Icon className="h-3 w-3 flex-shrink-0" />
          <span className="hidden sm:block">{current.label}</span>
          <ChevronDownIcon
            className="h-2 w-2 flex-shrink-0"
            aria-hidden="true"
          />
        </ListboxButton>
        <ListboxOptions className="min-w-36 max-w-48">
          {PATTERNS.map((p) => {
            const config = PATTERN_CONFIG[p];
            const PatternIcon = config.icon;
            return (
              <ListboxOption key={p} value={p} className="gap-1">
                <div className="flex flex-row items-center gap-1.5">
                  <PatternIcon className="h-3 w-3 flex-shrink-0" />
                  <span>{config.label}</span>
                </div>
                {pattern === p && <CheckIcon className="ml-auto h-3 w-3" />}
              </ListboxOption>
            );
          })}
        </ListboxOptions>
      </div>
    </Listbox>
  );
}
