/**
 * 权限校验插件
 */

import { FastifyPluginAsync } from "fastify";

import faCookie from "@fastify/cookie";
import faSession from "@fastify/session";
import faRawBody from "fastify-raw-body";

import { AesEncrypt, AesDecrypt } from "~/utils/crypto";

const defaultSessionSecret = "q6NlSjRdxE2Stbm4L5bN2z3HAo2HQsO6";
const defaultCookieSecret = "0tzLQoda2Swt5T1uy0kSiiwxMy0T3i4i";

const CheckTimeExpired = (time: Date) => {
    return time.getTime() >= new Date().getTime();
};

const authPlugin : FastifyPluginAsync = async (fastify, options) => {
    // Cookie
    await fastify.register(faCookie, {
        secret: process.env.COOKIE_SECRET || defaultCookieSecret,
    });
    await fastify.register(faSession, {secret: process.env.SESSION_SECRET || defaultSessionSecret});
    await fastify.register(faRawBody, {
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
                    await fastify.prisma.session.delete({
                        where: {
                            id: session.id
                        }
                    });
                    reply.clearCookie("sessionId");
                    reply.send({
                        code: 301,
                        msg: "your session is expired, please login in again."
                    });
                    return;
                }
                if (request.rawBody && session.shared_key.length) {
                    request.rawBody = AesDecrypt(request.rawBody.toString(), session.shared_key);
                }
                request.session.set<any>("sharedKey", session.shared_key);
                request.session.set<any>("userId", session.user_id);
                reply.cookies = request.cookies;
                return;
            }
        } catch (e) {
            reply.send({
                code: 401,
                msg: "you are not logged in."
            });
            return;
        }
        reply.clearCookie("sessionId");
        reply.send({
            code: 401,
            msg: "you are not logged in."
        });
    });

    fastify.addHook("onSend", async (request, reply, payload) => {
        const sharedKey = request.session.get<any>("sharedKey");
        if (sharedKey && payload) {
            reply.header("Content-Type", "text/plain;charset=utf-8");
            return AesEncrypt(payload, sharedKey);
        }
    });

    // Routes
    fastify.get("/auth", async(req, reply) => {
        return {
            code: 200,
            data: {
                name: "auth"
            }
        };
    });
};

export default authPlugin;

