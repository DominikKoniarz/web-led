import path from "path";
import { pathToFileURL } from "url";

const docPath = path.resolve("docs", "api", "index.html");
const fileUrl = pathToFileURL(docPath).href;

console.log("API docs generated.");
console.log(`Path: ${docPath}`);
console.log(`URL:  ${fileUrl}`);
