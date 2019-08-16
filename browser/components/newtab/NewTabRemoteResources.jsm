/* exported NewTabRemoteResources */

"use strict";

this.EXPORTED_SYMBOLS = ["NewTabRemoteResources"];

const NewTabRemoteResources = {
  MODE_CHANNEL_MAP: {
    production: {origin: "https://content.cdn.mozilla.net"},
    staging: {origin: "https://s3_proxy_tiles.stage.mozaws.net"},
    dev: {origin: "http://localhost:8888"}
  }
};
