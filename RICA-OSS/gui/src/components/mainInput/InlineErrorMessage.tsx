import { useContext } from "react";
import { IdeMessengerContext } from "../../context/IdeMessenger";
import { useAppDispatch, useAppSelector } from "../../redux/hooks";
import { setInlineErrorMessage } from "../../redux/slices/sessionSlice";

export type InlineErrorMessageType = "out-of-context";

export default function InlineErrorMessage() {
  const dispatch = useAppDispatch();
  const ideMessenger = useContext(IdeMessengerContext);
  const inlineErrorMessage = useAppSelector(
    (state) => state.session.inlineErrorMessage,
  );
  if (inlineErrorMessage === "out-of-context") {
    return (
      <div
        className={`border-border relative m-2 flex flex-col rounded-md border border-solid bg-transparent p-4`}
      >
        <p className={`thread-message text-error text-center`}>
          {`Message exceeds context limit.`}
        </p>
        <div className="text-description flex flex-row items-center justify-center gap-1.5 px-3">
          {/* Phase 5.1.5 (Bug 1 fix): the upstream "Open config" link
              posted `config/openProfile` → ConfigHandler.openConfigProfile,
              which for non-local profiles built `${env.APP_URL}${profileId}`
              → `https://continue.dev/rica/dynamic-models`. RICA models are
              centrally managed by Sang's backend; there is no per-user
              config file to open. Replace with an info toast. The
              defense-in-depth short-circuit in
              ConfigHandler.openConfigProfile lives in 5.1.6. */}
          <div
            className="cursor-pointer text-xs hover:underline"
            onClick={() => {
              ideMessenger.post("showToast", [
                "info",
                "RICA models are managed centrally — try a shorter prompt.",
              ]);
            }}
          >
            <span className="xs:flex hidden">Why?</span>
            <span className="xs:hidden">Why?</span>
          </div>
          |
          <span
            className="cursor-pointer text-xs hover:underline"
            onClick={() => {
              dispatch(setInlineErrorMessage(undefined));
            }}
          >
            Hide
          </span>
        </div>
      </div>
    );
  }
  return null;
}
