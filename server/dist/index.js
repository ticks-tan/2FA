"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const server_1 = require("./server");
server_1.serverApp.listen({
    port: 3000,
    host: "127.0.0.1",
}, (err, address) => {
    if (err) {
        server_1.serverApp.log.error(err);
        process.exit(1);
    }
    server_1.serverApp.log.info(`server listen on ${address}`);
});
