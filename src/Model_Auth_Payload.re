type t = {githubToken: string};

let decode = (json: Js.Json.t): Js.Promise.t(t) =>
  Js.Promise.make((~resolve, ~reject) =>
    switch (Json.Decode.{githubToken: json |> field("github_token", string)}) {
    | payload => resolve(. payload)
    | exception e => reject(. e)
    }
  );