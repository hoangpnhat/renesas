import CustomizeMainView from "./CustomizeMainView.tsx";
import { useCustomizePage } from "./hooks/useCustomizePage.ts";

const RuleMainView = () => {
  const pageProps = useCustomizePage("rule");
  return <CustomizeMainView type="rule" {...pageProps} />;
};

export default RuleMainView;
