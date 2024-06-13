
import Fastify from "fastify";

import faAuth from "~/plugins/auth";
import faPrisma from "~/plugins/prisma";

export const serverApp = Fastify({
    logger: true
});

// Prisma
serverApp.register(faPrisma);

// Auth
serverApp.register(faAuth);
