import { useEffect, useState } from "react";

export const useMountTime = () => {
  const [mountTime, setMountTime] = useState(0);
  useEffect(() => {
    setMountTime(mountTime + 1);
  }, []);
  return mountTime;
};
