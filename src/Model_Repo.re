type t = {
  id: int,
  name: string,
  description: option(string),
  forksCount: int,
  stargazersCount: int,
  owner: Model_User.t,
};

let decode: Js.Json.t => array(t) =
  json =>
    Json.Decode.(
      json
      |> array(item =>
           {
             id: item |> field("id", int),
             name: item |> field("name", string),
             description: item |> optional(field("description", string)),
             forksCount: item |> field("forks_count", int),
             stargazersCount: item |> field("stargazers_count", int),
             owner: item |> field("owner", Model_User.decode),
           }
         )
    );