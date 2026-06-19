import React from "react";
import renesasLogo from "../assets/logo/renesas.png";

export const LayoutHeaderComponent = () => {
  return (
    <React.Fragment>
      <header className="w-full h-[5%] sticky text-white">
        <nav className="bg-primary-main h-full z-20 top-0 start-0 border-b border-border-default flex justify-around items-center">
          <div className=" h-full flex flex-wrap items-center justify-between mx-auto">
            <a
              href=""
              className="flex items-center space-x-3 rtl:space-x-reverse"
            >
              <img src={renesasLogo} className="h-8" alt="Renesas Logo" />
            </a>
          </div>
        </nav>
      </header>
    </React.Fragment>
  );
};
