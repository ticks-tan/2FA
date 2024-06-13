"use strict";
/**
 * 权限校验
 */
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const cookie_1 = __importDefault(require("@fastify/cookie"));
const session_1 = __importDefault(require("@fastify/session"));
const fastify_raw_body_1 = __importDefault(require("fastify-raw-body"));
const crypto_1 = require("../utils/crypto");
const defaultSessionSecret = "q6NlSjRdxE2Stbm4L5bN2z3HAo2HQsO6";
const defaultCookieSecret = "0tzLQoda2Swt5T1uy0kSiiwxMy0T3i4i";
const CheckTimeExpired = (time) => {
    return time.getTime() >= new Date().getTime();
};
const authPlugin = async (fastify, options) => {
    // Cookie
    await fastify.register(cookie_1.default, {
        secret: process.env.COOKIE_SECRET || defaultCookieSecret,
    });
    await fastify.register(session_1.default, { secret: process.env.SESSION_SECRET || defaultSessionSecret });
    await fastify.register(fastify_raw_body_1.default, {
        field: "rawBody",
        global: true,
        encoding: "utf8"
    });
    fastify.addHook("preHandler", async (request, reply) => {
        const { sessionId } = request.cookies;
        if (!sessionId) {
            reply.code(401).send({
                code: 401,
                msg: "you are not logged in."
            });
            return;
        }
        try {
            const session = await fastify.prisma.session.findUnique({
                where: {
                    id: Number.parseInt(sessionId)
                },
                select: {
                    id: true,
                    user_id: true,
                    shared_key: true,
                    expired: true
                }
            });
            if (session) {
                if (CheckTimeExpired(session.expired)) {
                    reply.send({
                        code: 301,
                        msg: "your session is expired, please login in again."
                    });
                    return;
                }
                if (request.rawBody && session.shared_key.length) {
                    request.rawBody = (0, crypto_1.AesDecrypt)(request.rawBody.toString(), session.shared_key);
                }
                request.session.set("sharedKey", session.shared_key);
                request.session.set("userId", session.user_id);
                return;
            }
        }
        catch (e) {
            reply.send({
                code: 401,
                msg: "you are not logged in."
            });
            return;
        }
        reply.send({
            code: 401,
            msg: "you are not logged in."
        });
    });
    fastify.addHook("onSend", async (request, reply, payload) => {
        const sharedKey = request.session.get("sharedKey");
        if (sharedKey && payload) {
            return (0, crypto_1.AesEncrypt)(payload, sharedKey);
        }
    });
    // Routes
    fastify.get("/auth", async (req, reply) => {
        return {
            code: 200,
            data: {
                name: "auth"
            }
        };
    });
};
exports.default = authPlugin;
