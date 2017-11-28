# Temperature Node Example
Simple temperature node example using Arduino UNO and the RN2483 LoRa module


## TTN payload format

```js
function Decoder(bytes, port) {
  return {
     'temperature' : (bytes[0] << 8) + bytes[1]
  };
}
```
