type t = {
  name: string,
  login: string,
  image: string,
};

let decode: Js.Json.t => t =
  json =>
    Json.Decode.{
      name: json |> field("name", string),
      login: json |> field("login", string),
      image: json |> field("image", string),
    };