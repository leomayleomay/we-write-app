[@react.component]
let make = (~token) => {
  React.useEffect0(() => {
    Js.Promise.(
      token->Model_Auth_Token.decode
      |> then_(Model_Auth_Payload.decode)
      |> then_((payload: Model_Auth_Payload.t) => {
           token->Auth.Token.set->ignore;
           payload.githubToken->Auth.GithubToken.set->ignore;
           ReasonReact.Router.push("/")->resolve;
         })
      |> catch(_ => ReasonReact.Router.push("/auth/failure")->resolve)
      |> ignore
    );

    None;
  });
  "Authenticating ..." |> React.string;
};