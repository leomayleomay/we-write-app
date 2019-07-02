[@bs.module] external uuid: unit => string = "uuid/v4";

let headers = () =>
  Axios.Headers.fromObj({
    "Authorization": "token " ++ Belt.Option.getExn(Auth.GithubToken.get()),
  });

let createRepo = (~params) => {
  Axios.postDatac(
    "https://api.github.com/user/repos",
    params,
    Axios.makeConfig(~headers=headers(), ()),
  );
};

let updateRepo = (~repo, ~params) => {
  Axios.postDatac(
    {j|https://api.github.com/repos/$repo|j},
    params,
    Axios.makeConfig(~headers=headers(), ()),
  );
};

let createFork = (~repo) => {
  Axios.postDatac(
    {j|https://api.github.com/repos/$repo/forks|j},
    Js.Obj.empty(),
    Axios.makeConfig(~headers=headers(), ()),
  );
};

let createBranch = (~repo, ~sha) => {
  let newBranch = "patch-" ++ uuid();

  Js.Promise.(
    Axios.postDatac(
      {j|https://api.github.com/repos/$repo/git/refs|j},
      {"ref": "refs/heads/" ++ newBranch, "sha": sha},
      Axios.makeConfig(~headers=headers(), ()),
    )
    |> then_(_ => newBranch |> resolve)
  );
};

let getPullRequestFiles = (~repo, ~number) => {
  Axios.getc(
    {j|https://api.github.com/repos/$repo/pulls/$number|j},
    Axios.makeConfig(
      ~headers=
        Axios.Headers.fromObj({
          "Authorization":
            "Bearer " ++ Belt.Option.getExn(Auth.GithubToken.get()),
          "Accept": "application/vnd.github.v3.diff",
        }),
      (),
    ),
  );
};

let createFile = (~repo, ~branch, ~path, ~message, ~content) => {
  Axios.putDatac(
    {j|https://api.github.com/repos/$repo/contents/$path|j},
    {
      "branch": branch,
      "message": message,
      "content": Base64.encode(content),
    },
    Axios.makeConfig(~headers=headers(), ()),
  );
};

let updateFile = (~repo, ~branch, ~sha, ~path, ~message, ~content) => {
  Axios.putDatac(
    {j|https://api.github.com/repos/$repo/contents/$path|j},
    {
      "sha": sha,
      "branch": branch,
      "message": message,
      "content": Base64.encode(content),
    },
    Axios.makeConfig(~headers=headers(), ()),
  );
};

let deleteFile = (~repo, ~branch, ~sha, ~path, ~message) => {
  Axios.deletec(
    {j|https://api.github.com/repos/$repo/contents/$path?sha=$sha&branch=$branch&message=|j}
    ++ Js.Global.encodeURI(message),
    Axios.makeConfig(~headers=headers(), ()),
  );
};