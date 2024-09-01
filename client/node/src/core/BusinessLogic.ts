const MIN_USERNAME_LENGTH = 5;
const MAX_USERNAME_LENGTH = 15;
const MIN_PASSWORD_LENGTH = 8;
const MAX_PASSWORD_LENGTH = 50;

export class BusinessLogic {
  public validateLength(
    length: number,
    type: "username" | "password"
  ): boolean {
    if (type === "username") {
      return length >= MIN_USERNAME_LENGTH && length <= MAX_USERNAME_LENGTH;
    } else {
      return length >= MIN_PASSWORD_LENGTH && length <= MAX_PASSWORD_LENGTH;
    }
  }
}
