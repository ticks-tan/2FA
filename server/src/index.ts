import { serverApp } from "~/server";

serverApp.listen(
	{
		port: 3000,
		host: "127.0.0.1",
	},
	(err, address) => {
		if (err) {
			serverApp.log.error(err);
			process.exit(1);
		}
		serverApp.log.info(`server listen on ${address}`);
	}
);
