import CryptoJS from "crypto-js";

export function AesEncrypt(data: string | object, key: string): string {
	try {
		const kIv = CryptoJS.lib.WordArray.random(16);
		// console.log(`iv: ${kIv.toString()}`);
		if (typeof data === "string") {
			const text = CryptoJS.enc.Utf8.parse(data);
			const encrypted = CryptoJS.AES.encrypt(
				text,
				CryptoJS.enc.Utf8.parse(key),
				{
					iv: kIv,
					mode: CryptoJS.mode.CBC,
					padding: CryptoJS.pad.Pkcs7,
				}
			);
			return kIv
				.concat(encrypted.ciphertext)
				.toString(CryptoJS.enc.Base64);
		} else if (typeof data === "object") {
			const text = CryptoJS.enc.Utf8.parse(JSON.stringify(data));
			const encrypted = CryptoJS.AES.encrypt(
				text,
				CryptoJS.enc.Utf8.parse(key),
				{
					iv: kIv,
					mode: CryptoJS.mode.CBC,
					padding: CryptoJS.pad.Pkcs7,
				}
			);
			return kIv.concat(encrypted.ciphertext).toString(CryptoJS.enc.Base64);
		}
	} catch (_e) {}
	return "";
}

export function AesDecrypt(data: string, key: string) {
	try {
		const text = CryptoJS.enc.Base64.parse(data);
		const kIv = CryptoJS.lib.WordArray.create(text.words.slice(0, 4));
		const ciphertext = CryptoJS.lib.WordArray.create(
			text.words.slice(4)
		).toString(CryptoJS.enc.Base64);
		const decrypted = CryptoJS.AES.decrypt(
			ciphertext,
			CryptoJS.enc.Utf8.parse(key),
			{
				iv: kIv,
				mode: CryptoJS.mode.CBC,
				padding: CryptoJS.pad.Pkcs7,
			}
		);
		return decrypted.toString(CryptoJS.enc.Utf8);
	} catch (_e) {
		console.log(_e);
	}
	return "";
}
