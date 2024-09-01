import { UserInterface } from "./ui/UserInterface";
import { BusinessLogic } from "./core/BusinessLogic";
import { SocketsInterface } from "./core/SocketsInterface";

const SERVER_IP = "127.0.0.1";
const SERVER_PORT = 8080;

const userInterface = new UserInterface();
const businessLogic = new BusinessLogic();
const networkClient = new SocketsInterface();

function showMenu() {
  userInterface.showMenu((choice: number) => {
    switch (choice) {
      case 1:
        handleUsernameRequest();
        break;

      case 2:
        handlePasswordRequest();
        break;

      case 3:
        exit();
        break;

      default:
        console.log("Invalid choice. Please try again.");
        showMenu();
    }
  });
}

function handleUsernameRequest() {
  userInterface.askForLength(
    `Enter desired username length (5-15): `,
    (length) => {
      if (businessLogic.validateLength(length, "username")) {
        networkClient.sendRequest(`username:${length}`);
        networkClient.receiveResponse((response: any) => {
          console.log("Server response:", response);
          showMenu();
        });
      } else {
        console.log(`Invalid length. Please enter a number between 5 and 15.`);
        showMenu();
      }
    }
  );
}

function handlePasswordRequest() {
  userInterface.askForLength(
    `Enter desired password length (8-50): `,
    (length) => {
      if (businessLogic.validateLength(length, "password")) {
        networkClient.sendRequest(`password:${length}`);
        networkClient.receiveResponse((response: any) => {
          console.log("Server response:", response);
          showMenu();
        });
      } else {
        console.log(`Invalid length. Please enter a number between 8 and 50.`);
        showMenu();
      }
    }
  );
}

function exit() {
  console.log("Exiting...");
  userInterface.close();
  networkClient.close();
  process.exit(0);
}

networkClient.connect(
  SERVER_PORT,
  SERVER_IP,
  () => {
    showMenu();
  },
  (err: any) => {
    console.error("Connection error:", err.message);
  }
);
