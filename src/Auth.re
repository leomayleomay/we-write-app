module Token =
  LocalStorage.Operation({
    let key = "we-write-app:token";
  });

module GithubToken =
  LocalStorage.Operation({
    let key = "we-write-app:github_token";
  });

let isAuthenticated = () => Token.get()->Belt.Option.isSome;

let resetStore = () => {
  Token.remove();

  GithubToken.remove();
};