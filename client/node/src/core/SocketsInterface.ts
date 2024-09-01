import * as net from "net";

export class SocketsInterface {
  private client: net.Socket;

  constructor() {
    this.client = new net.Socket();
  }

  public connect(
    port: number,
    host: string,
    onConnect: () => void,
    onError: (err: Error) => void
  ): void {
    this.client.connect(port, host, () => {
      console.log(`Connected to server ${host} on port ${port}.`);
      onConnect();
    });

    this.client.on("error", onError);
    this.client.on("close", () => {
      console.log("Connection closed");
    });
  }

  public sendRequest(request: string): void {
    this.client.write(request, (err) => {
      if (err) {
        console.error("Failed to send request:", err.message);
      }
    });
  }

  public receiveResponse(callback: (data: string) => void): void {
    this.client.once("data", (data) => {
      callback(data.toString().trim());
    });
  }

  public close(): void {
    this.client.end();
  }
}
