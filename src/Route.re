type t =
  | Home
  | Auth_Github
  | Auth_Failure
  | Auth_Callback(string)
  | Auth_Logout
  | MyRepos_Index
  | Repo_Files_Index(string, string)
  | Repo_Files_New(string, string)
  | Repo_Files_Edit(string, string, string)
  | Repo_Files_Show(string, string, string)
  | Repo_Files_Delete(string, string, string)
  | PullRequests_Index(string, string)
  | PullRequests_Show(string, string, int)
  | PullRequests_New(string, string, string, string)
  | NotFound;

let urlToRoute: ReasonReact.Router.url => t =
  url =>
    switch (url.path) {
    | []
    | [""]
    | ["/"] => Home
    | ["auth", "github"] => Auth_Github
    | ["auth", "failure"] => Auth_Failure
    | ["auth", "callback"] =>
      let parsedQueryString = url.search->URLSearchParams.make;
      try (
        Auth_Callback(
          parsedQueryString
          ->(URLSearchParams.get("token"))
          ->Belt.Option.getExn,
        )
      ) {
      | _ => NotFound
      };
    | ["auth", "logout"] => Auth_Logout
    | ["my", "repos"] => MyRepos_Index
    | [owner, name] => Repo_Files_Index(owner, name)
    | [owner, name, "blob", "master", path] =>
      Repo_Files_Show(owner, name, path)
    | [owner, name, "new", "master"] => Repo_Files_New(owner, name)
    | [owner, name, "edit", "master", path] =>
      Repo_Files_Edit(owner, name, path)
    | [owner, name, "delete", "master", path] =>
      Repo_Files_Delete(owner, name, path)
    | [owner, name, "pulls"] => PullRequests_Index(owner, name)
    | [owner, name, "pull", number] =>
      PullRequests_Show(owner, name, int_of_string(number))

    | [owner, name, "compare", baseRefName, headRefName] =>
      PullRequests_New(owner, name, baseRefName, headRefName)
    | _ => NotFound
    };