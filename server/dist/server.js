"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.serverApp = void 0;
const fastify_1 = __importDefault(require("fastify"));
const auth_1 = __importDefault(require("./plugins/auth"));
const prisma_1 = __importDefault(require("./plugins/prisma"));
exports.serverApp = (0, fastify_1.default)({
    logger: true
});
// Prisma
exports.serverApp.register(prisma_1.default);
// Auth
exports.serverApp.register(auth_1.default);
