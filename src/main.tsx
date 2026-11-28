import { StrictMode } from "react";
import { hydrateRoot } from "react-dom/client";
import "./styles/tokens.css";
import "./styles/site.css";
import App from "./App";

hydrateRoot(
  document.getElementById("root")!,
  <StrictMode>
    <App />
  </StrictMode>,
);
