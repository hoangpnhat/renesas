import CustomizeMainView from "./CustomizeMainView.tsx";
import { useCustomizePage } from "./hooks/useCustomizePage.ts";

const PromptMainView = () => {
  const pageProps = useCustomizePage("prompt");
  return <CustomizeMainView type="prompt" {...pageProps} />;
};

export default PromptMainView;
