import { XMarkIcon } from "@heroicons/react/24/outline";
import {
  FloatingArrow,
  arrow,
  autoUpdate,
  flip,
  offset,
  shift,
  useFloating,
} from "@floating-ui/react";
import { useEffect, useRef, useState } from "react";
import { useAppSelector } from "../../redux/hooks";
import {
  getLocalStorage,
  setLocalStorage,
} from "../../util/localStorage";

const TOUR_STORAGE_KEY = "hasSeenMultiAgentTour_v1";

// TESTING FLAG: when true, the tour appears on EVERY new chat regardless
// of localStorage. Flip back to true for QA / re-testing the tour flow.
const TESTING_ALWAYS_SHOW = false;

const ANCHOR_RETRY_INTERVAL_MS = 200;
const ANCHOR_RETRY_MAX_TRIES = 25; // ~5s total
const STEP_INITIAL_DELAY_MS = 800;

type Lang = "en" | "ja";

type StepLocalized = {
  title: string;
  body: React.ReactNode;
};

type StepDef = {
  id: 1 | 2 | 3;
  anchorSelector: string;
  /** When false, the Next button is hidden (e.g., step 1 advances on mode change) */
  showNext?: boolean;
  content: Record<Lang, StepLocalized>;
};

const STEPS: StepDef[] = [
  {
    id: 1,
    anchorSelector: '[data-testid="mode-select-button"]',
    showNext: false, // advances when mode flips to "multi-agent"
    content: {
      en: {
        title: "Try Multi-Agent mode",
        body: (
          <div>
            <span className="font-semibold">Multi-Agent</span> coordinates
            specialized sub-agents (Researcher, Implementer, Reviewer, …) on
            complex tasks.{" "}
            <span className="font-semibold">
              Click the mode selector and pick "Multi-Agent"
            </span>{" "}
            to continue this tour.
          </div>
        ),
      },
      ja: {
        title: "マルチエージェントモードを試す",
        body: (
          <div>
            <span className="font-semibold">マルチエージェント</span>
            は、複雑なタスクに特化したサブエージェント
            (Researcher、Implementer、Reviewer など) を連携させます。
            <span className="font-semibold">
              モードセレクタをクリックして「Multi-Agent」を選択
            </span>
            して、ツアーを続けてください。
          </div>
        ),
      },
    },
  },
  {
    id: 2,
    anchorSelector: '[data-testid="workflow-pattern-select-button"]',
    showNext: true,
    content: {
      en: {
        title: "Pick a workflow pattern",
        body: (
          <div className="space-y-1.5">
            <div>This dropdown picks how agents are coordinated:</div>
            <ul className="ml-3 list-disc space-y-1 text-[11px]">
              <li>
                <span className="font-semibold">Auto</span> — orchestrator
                decides based on your task
              </li>
              <li>
                <span className="font-semibold">Sequential</span> — one agent
                at a time, passing context forward
              </li>
              <li>
                <span className="font-semibold">Parallel</span> — multiple
                agents fan out simultaneously on independent subtasks
              </li>
              <li>
                <span className="font-semibold">Iterative</span> — test → fix
                → re-test loop with retries
              </li>
              <li>
                <span className="font-semibold">Collaborative</span> — multiple
                reviewers on the same target with different focus
              </li>
            </ul>
          </div>
        ),
      },
      ja: {
        title: "ワークフローパターンを選択",
        body: (
          <div className="space-y-1.5">
            <div>
              このドロップダウンでエージェントの連携方法を選びます:
            </div>
            <ul className="ml-3 list-disc space-y-1 text-[11px]">
              <li>
                <span className="font-semibold">Auto</span> —
                タスクに応じてオーケストレータが自動決定
              </li>
              <li>
                <span className="font-semibold">Sequential</span> —
                エージェントを 1 つずつ、コンテキストを引き継いで実行
              </li>
              <li>
                <span className="font-semibold">Parallel</span> —
                複数のエージェントが独立したサブタスクを同時並行で実行
              </li>
              <li>
                <span className="font-semibold">Iterative</span> — テスト
                → 修正 → 再テストのループ (リトライ付き)
              </li>
              <li>
                <span className="font-semibold">Collaborative</span> —
                同一ターゲットを複数のレビュアーが異なる観点でレビュー
              </li>
            </ul>
          </div>
        ),
      },
    },
  },
  {
    id: 3,
    anchorSelector: '[data-testid^="continue-input-box-"]',
    showNext: true,
    content: {
      en: {
        title: "Now describe your task",
        body: (
          <div>
            Type a task here and hit send. The orchestrator analyzes it,
            plans sub-agents, and reports back when they're done.
            <div className="mt-2 text-[11px] opacity-80">
              Tip: be specific. Mention file paths or directories to keep
              agents focused.
            </div>
          </div>
        ),
      },
      ja: {
        title: "タスクを記述してください",
        body: (
          <div>
            ここにタスクを入力して送信してください。オーケストレータが内容を解析し、
            サブエージェントを計画し、完了時に報告します。
            <div className="mt-2 text-[11px] opacity-80">
              ヒント: 具体的に書いてください。ファイルパスやディレクトリを指定すると、
              エージェントの焦点がぶれません。
            </div>
          </div>
        ),
      },
    },
  },
];

const UI_STRINGS: Record<Lang, {
  stepLabel: (current: number, total: number) => string;
  brand: string;
  skip: string;
  next: string;
  done: string;
  closeAria: string;
}> = {
  en: {
    stepLabel: (c, t) => `Step ${c} of ${t}`,
    brand: "Multi-Agent tour",
    skip: "Skip",
    next: "Next",
    done: "Done",
    closeAria: "Close tour",
  },
  ja: {
    stepLabel: (c, t) => `ステップ ${c} / ${t}`,
    brand: "マルチエージェントツアー",
    skip: "スキップ",
    next: "次へ",
    done: "完了",
    closeAria: "ツアーを閉じる",
  },
};

/**
 * Multi-step guided tour pointing users through Multi-Agent mode setup.
 * Replaces the earlier single-step MultiAgentNudge.
 *
 * Steps:
 *   1. Select Multi-Agent mode (anchor: mode-select-button)
 *   2. Pick a workflow pattern (anchor: workflow-pattern-select-button)
 *   3. Type a task in the input box (anchor: continue-input-box-*)
 *
 * Persistence: localStorage key `hasSeenMultiAgentTour_v1`. Once finished
 * (Done) or skipped (X), it never reappears unless the key is cleared.
 *
 * Testing knob: TESTING_ALWAYS_SHOW=true ignores localStorage and shows the
 * tour on every new chat. Flip to false before shipping clean.
 */
export default function MultiAgentNudge() {
  const mode = useAppSelector((s) => s.session.mode);
  const sessionModelLock = useAppSelector((s) => s.session.sessionModelLock);
  const sessionId = useAppSelector((s) => s.session.id);

  const [stepIndex, setStepIndex] = useState(0);
  const [visible, setVisible] = useState(false);
  const [anchor, setAnchor] = useState<HTMLElement | null>(null);
  const [lang, setLang] = useState<Lang>("en");
  const arrowRef = useRef<SVGSVGElement | null>(null);
  const dismissedRef = useRef(false);

  // Mode ref so timer / retry callbacks can read latest mode without
  // capturing a stale closure.
  const modeRef = useRef(mode);
  useEffect(() => {
    modeRef.current = mode;
  }, [mode]);

  const currentStep = STEPS[stepIndex];
  const ui = UI_STRINGS[lang];
  const localized = currentStep?.content[lang];

  // Initial decision: should the tour appear at all? Re-evaluates whenever
  // the session changes (new chat, switched to past chat) so the tour can
  // re-appear on a fresh session even after being dismissed in a prior one.
  // Chat.tsx doesn't unmount this component on session change — it just
  // resets internal Redux state — so without sessionId in deps the
  // dismissedRef would block the tour forever after first dismissal.
  useEffect(() => {
    // Reset on every session change so a fresh chat starts with a clean
    // tour state regardless of what the user did in the previous session.
    dismissedRef.current = false;
    setVisible(false);
    setAnchor(null);
    setStepIndex(0);

    if (!TESTING_ALWAYS_SHOW) {
      const seen = getLocalStorage(TOUR_STORAGE_KEY);
      if (seen === true) return;
      // If user already used multi-agent on this session, no point starting
      // the tour — they're past step 1 anyway.
      if (sessionModelLock !== null) return;
    }

    const t = setTimeout(() => {
      if (dismissedRef.current) return; // user could have skipped during the wait
      // Re-check mode at fire time. Step 1's job is to send the user TO
      // multi-agent — if they're already there at startup, skip step 1.
      const startStep = modeRef.current === "multi-agent" ? 1 : 0;
      setStepIndex(startStep);
      setVisible(true);
    }, STEP_INITIAL_DELAY_MS);
    return () => clearTimeout(t);
  }, [sessionId, sessionModelLock]);

  // Step 1 → 2 auto-advance: when user flips mode to multi-agent during
  // step 1, advance.
  useEffect(() => {
    if (!visible) return;
    if (currentStep?.id === 1 && mode === "multi-agent") {
      setStepIndex(1);
    }
  }, [mode, visible, currentStep?.id]);

  // Anchor resolution: each time stepIndex changes, find the new anchor.
  // Some anchors only mount conditionally (e.g., workflow-pattern-select
  // only renders in multi-agent mode), so retry briefly until it appears.
  useEffect(() => {
    if (!visible) return;
    if (!currentStep) return;
    let tries = 0;
    let interval: ReturnType<typeof setInterval> | null = null;

    const tryFind = () => {
      const el = document.querySelector<HTMLElement>(
        currentStep.anchorSelector,
      );
      if (el) {
        setAnchor(el);
        if (interval) {
          clearInterval(interval);
          interval = null;
        }
        return true;
      }
      return false;
    };

    if (!tryFind()) {
      interval = setInterval(() => {
        tries += 1;
        if (tryFind() || tries >= ANCHOR_RETRY_MAX_TRIES) {
          if (interval) clearInterval(interval);
          interval = null;
        }
      }, ANCHOR_RETRY_INTERVAL_MS);
    }

    return () => {
      if (interval) clearInterval(interval);
    };
  }, [stepIndex, visible, currentStep]);

  const { refs, floatingStyles, context } = useFloating({
    open: visible,
    onOpenChange: setVisible,
    // Default to "top" — RICA's input bar sits at the bottom of the
    // viewport, and the mode + workflow + input anchors live inside it.
    // Putting the tooltip above the anchor keeps it inside the viewport
    // and visually "above" the controls. flip() handles edge cases.
    placement: "top",
    middleware: [
      offset(12),
      flip({ fallbackPlacements: ["bottom", "top-start", "bottom-start"] }),
      shift({ padding: 12 }),
      arrow({ element: arrowRef }),
    ],
    whileElementsMounted: autoUpdate,
    elements: { reference: anchor },
  });

  if (!visible || !anchor || !currentStep || !localized) return null;

  const isLast = stepIndex >= STEPS.length - 1;

  const finish = () => {
    dismissedRef.current = true;
    setVisible(false);
    setAnchor(null);
    if (!TESTING_ALWAYS_SHOW) {
      setLocalStorage(TOUR_STORAGE_KEY, true);
    }
  };

  const skip = () => finish();

  const next = () => {
    if (isLast) {
      finish();
      return;
    }
    setStepIndex(stepIndex + 1);
  };

  // Inline style helpers — VS Code's webview injects default <button>
  // styling that fights Tailwind on bg-color and text-color. Inline
  // styles win the cascade and look consistent across themes.
  const baseBtnStyle: React.CSSProperties = {
    border: "none",
    cursor: "pointer",
    fontFamily: "inherit",
    fontWeight: 600,
    fontSize: "11px",
    padding: "5px 12px",
    borderRadius: "5px",
    transition: "transform 140ms ease, box-shadow 140ms ease, background-color 140ms ease, filter 140ms ease",
  };
  // Skip = ghost / secondary affordance (clear escape hatch but doesn't compete)
  const skipBtnStyle: React.CSSProperties = {
    ...baseBtnStyle,
    backgroundColor: "transparent",
    color: "rgba(255, 255, 255, 0.85)",
    border: "1px solid rgba(255, 255, 255, 0.35)",
  };
  // Next/Done = primary CTA, warm amber pops against the blue tour bg
  const nextBtnStyle: React.CSSProperties = {
    ...baseBtnStyle,
    backgroundColor: "#fbbf24", // amber-400
    color: "#1e3a8a",            // blue-900
    boxShadow: "0 2px 0 rgba(180, 83, 9, 0.55), 0 4px 14px rgba(251, 191, 36, 0.35)",
  };
  const xBtnStyle: React.CSSProperties = {
    border: "none",
    background: "transparent",
    color: "rgba(255, 255, 255, 0.85)",
    cursor: "pointer",
    padding: "4px",
    borderRadius: "4px",
    lineHeight: 0,
    transition: "background-color 120ms ease",
  };
  const langBtnBase: React.CSSProperties = {
    border: "1px solid rgba(255, 255, 255, 0.25)",
    background: "transparent",
    color: "rgba(255, 255, 255, 0.85)",
    cursor: "pointer",
    padding: "1px 6px",
    fontSize: "10px",
    fontWeight: 600,
    borderRadius: "3px",
    transition: "background-color 120ms ease, color 120ms ease",
  };
  const langBtnActive: React.CSSProperties = {
    ...langBtnBase,
    backgroundColor: "#ffffff",
    color: "#1d4ed8",
    borderColor: "#ffffff",
  };

  return (
    <>
      <style>{ANIMATION_CSS}</style>
    <div
      ref={refs.setFloating}
      className="rica-tour-card"
      style={{
        ...floatingStyles,
        zIndex: 150,
        maxWidth: 320,
        background: "linear-gradient(135deg, #2563eb 0%, #3b82f6 60%, #6366f1 100%)",
        color: "#ffffff",
        border: "1px solid rgba(147, 197, 253, 0.6)",
        borderRadius: 10,
        padding: "10px 12px",
        boxShadow: "0 10px 30px rgba(0, 0, 0, 0.45), 0 0 0 1px rgba(255,255,255,0.05) inset",
        fontSize: 12,
        lineHeight: 1.45,
      }}
      role="dialog"
      aria-label={`${ui.brand}: ${localized.title}`}
    >
      <div style={{ display: "flex", gap: 8, alignItems: "flex-start" }}>
        <div style={{ minWidth: 0, flex: 1 }}>
          <div
            style={{
              display: "flex",
              alignItems: "center",
              justifyContent: "space-between",
              gap: 8,
              marginBottom: 4,
            }}
          >
            <span
              style={{
                fontSize: 11,
                fontWeight: 700,
                letterSpacing: "0.05em",
                textTransform: "uppercase",
                color: "rgba(255, 255, 255, 0.85)",
              }}
            >
              {ui.stepLabel(currentStep.id, STEPS.length)}
            </span>
            <div style={{ display: "flex", alignItems: "center", gap: 6 }}>
              <span style={{ fontSize: 10, color: "rgba(255, 255, 255, 0.6)" }}>
                {ui.brand}
              </span>
              <div style={{ display: "flex", gap: 2 }}>
                <button
                  onClick={() => setLang("en")}
                  className="rica-tour-lang-btn"
                  style={lang === "en" ? langBtnActive : langBtnBase}
                  aria-pressed={lang === "en"}
                >
                  EN
                </button>
                <button
                  onClick={() => setLang("ja")}
                  className="rica-tour-lang-btn"
                  style={lang === "ja" ? langBtnActive : langBtnBase}
                  aria-pressed={lang === "ja"}
                >
                  日本語
                </button>
              </div>
            </div>
          </div>
          <div
            key={`title-${stepIndex}-${lang}`}
            className="rica-tour-content"
            style={{
              fontSize: 13,
              fontWeight: 600,
              marginBottom: 4,
            }}
          >
            {localized.title}
          </div>
          <div
            key={`body-${stepIndex}-${lang}`}
            className="rica-tour-content"
            style={{ fontWeight: 500 }}
          >
            {localized.body}
          </div>
          <div
            style={{
              marginTop: 10,
              display: "flex",
              alignItems: "center",
              justifyContent: "flex-end",
              gap: 8,
            }}
          >
            <button
              onClick={skip}
              className="rica-tour-skip-btn"
              style={skipBtnStyle}
            >
              {ui.skip}
            </button>
            {currentStep.showNext && (
              <button
                onClick={next}
                className="rica-tour-next-btn"
                style={nextBtnStyle}
              >
                {isLast ? ui.done : ui.next}
              </button>
            )}
          </div>
        </div>
        <button
          onClick={skip}
          aria-label={ui.closeAria}
          className="rica-tour-x-btn"
          style={xBtnStyle}
        >
          <XMarkIcon style={{ width: 16, height: 16 }} />
        </button>
      </div>
      <FloatingArrow
        ref={arrowRef}
        context={context}
        className="rica-tour-arrow"
        style={{ fill: "#3b82f6" }}
        height={6}
        width={12}
      />
    </div>
    </>
  );
}

// ── Animation keyframes injected via <style> ──
// VS Code's webview doesn't ship Tailwind's animation utilities, and we
// already use inline styles to win the cascade. So a plain <style> block
// with @keyframes is the lowest-friction path.
const ANIMATION_CSS = `
@keyframes ricaTourEnter {
  0% {
    opacity: 0;
    transform: translateY(-6px) scale(0.96);
  }
  60% {
    opacity: 1;
    transform: translateY(1px) scale(1.01);
  }
  100% {
    opacity: 1;
    transform: translateY(0) scale(1);
  }
}
@keyframes ricaTourArrowPulse {
  0%, 100% { opacity: 1; transform: scale(1); }
  50%      { opacity: 0.65; transform: scale(1.4); }
}
@keyframes ricaTourBtnPulse {
  0%, 100% { box-shadow: 0 2px 0 rgba(180, 83, 9, 0.55), 0 4px 14px rgba(251, 191, 36, 0.35); }
  50%      { box-shadow: 0 2px 0 rgba(180, 83, 9, 0.55), 0 4px 22px rgba(251, 191, 36, 0.7); }
}
@keyframes ricaTourContentFade {
  0%   { opacity: 0; transform: translateY(2px); }
  100% { opacity: 1; transform: translateY(0); }
}
.rica-tour-content {
  animation: ricaTourContentFade 220ms cubic-bezier(0.22, 1, 0.36, 1);
}
.rica-tour-card {
  animation: ricaTourEnter 280ms cubic-bezier(0.22, 1, 0.36, 1);
  transform-origin: top left;
}
.rica-tour-arrow {
  transform-origin: center;
  animation: ricaTourArrowPulse 1.8s ease-in-out infinite;
}
.rica-tour-card .rica-tour-next-btn {
  animation: ricaTourBtnPulse 2.4s ease-in-out infinite;
}
.rica-tour-card .rica-tour-next-btn:hover,
.rica-tour-card .rica-tour-skip-btn:hover {
  filter: brightness(1.08);
  transform: translateY(-1px);
}
.rica-tour-card .rica-tour-next-btn:active,
.rica-tour-card .rica-tour-skip-btn:active {
  transform: translateY(0);
  filter: brightness(0.96);
}
.rica-tour-card .rica-tour-skip-btn:hover {
  background-color: rgba(255, 255, 255, 0.12) !important;
}
.rica-tour-card .rica-tour-x-btn:hover {
  background-color: rgba(255, 255, 255, 0.15) !important;
}
.rica-tour-card .rica-tour-lang-btn:hover:not([aria-pressed="true"]) {
  background-color: rgba(255, 255, 255, 0.12) !important;
}
`;

// To re-test the tour flow during QA: set TESTING_ALWAYS_SHOW = true
// (line 16). Default ships at false so the tour shows once per user
// (gated by localStorage `hasSeenMultiAgentTour_v1`).
