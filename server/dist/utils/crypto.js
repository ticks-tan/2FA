"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.AesDecrypt = exports.AesEncrypt = void 0;
const crypto_js_1 = __importDefault(require("crypto-js"));
function AesEncrypt(data, key) {
    try {
        const kIv = crypto_js_1.default.lib.WordArray.random(16);
        // console.log(`iv: ${kIv.toString()}`);
        if (typeof data === "string") {
            const text = crypto_js_1.default.enc.Utf8.parse(data);
            const encrypted = crypto_js_1.default.AES.encrypt(text, crypto_js_1.default.enc.Utf8.parse(key), {
                iv: kIv,
                mode: crypto_js_1.default.mode.CBC,
                padding: crypto_js_1.default.pad.Pkcs7,
            });
            return kIv
                .concat(encrypted.ciphertext)
                .toString(crypto_js_1.default.enc.Base64);
        }
        else if (typeof data === "object") {
            const text = crypto_js_1.default.enc.Utf8.parse(JSON.stringify(data));
            const encrypted = crypto_js_1.default.AES.encrypt(text, crypto_js_1.default.enc.Utf8.parse(key), {
                iv: kIv,
                mode: crypto_js_1.default.mode.CBC,
                padding: crypto_js_1.default.pad.Pkcs7,
            });
            return kIv.concat(encrypted.ciphertext).toString(crypto_js_1.default.enc.Base64);
        }
    }
    catch (_e) { }
    return "";
}
exports.AesEncrypt = AesEncrypt;
function AesDecrypt(data, key) {
    try {
        const text = crypto_js_1.default.enc.Base64.parse(data);
        const kIv = crypto_js_1.default.lib.WordArray.create(text.words.slice(0, 4));
        const ciphertext = crypto_js_1.default.lib.WordArray.create(text.words.slice(4)).toString(crypto_js_1.default.enc.Base64);
        const decrypted = crypto_js_1.default.AES.decrypt(ciphertext, crypto_js_1.default.enc.Utf8.parse(key), {
            iv: kIv,
            mode: crypto_js_1.default.mode.CBC,
            padding: crypto_js_1.default.pad.Pkcs7,
        });
        return decrypted.toString(crypto_js_1.default.enc.Utf8);
    }
    catch (_e) {
        console.log(_e);
    }
    return "";
}
exports.AesDecrypt = AesDecrypt;
