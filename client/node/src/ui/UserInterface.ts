import * as readline from "readline";

export class UserInterface {
  private rl: readline.Interface;

  constructor() {
    this.rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
    });
  }

  public showMenu(callback: (choice: number) => void): void {
    console.log("\n--- Menu ---");
    console.log("1. Generate Username");
    console.log("2. Generate Password");
    console.log("3. Exit");

    this.rl.question("Enter your choice: ", (answer) => {
      const choice = parseInt(answer.trim(), 10);
      callback(choice);
    });
  }

  public askForLength(
    prompt: string,
    callback: (length: number) => void
  ): void {
    this.rl.question(prompt, (answer) => {
      const length = parseInt(answer.trim(), 10);
      callback(length);
    });
  }

  public close(): void {
    this.rl.close();
  }
}
