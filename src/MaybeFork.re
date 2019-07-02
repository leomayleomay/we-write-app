let viewerPermission = (data: Js.t('a)) => {
  Belt.Option.(
    data##repository
    ->flatMap(repo => repo##viewerPermission)
    ->getWithDefault(`READ)
  );
};

let sha = (data: Js.t('a)) => {
  Belt.Option.(
    data##repository
    ->flatMap(repo => repo##ref)
    ->flatMap(ref =>
        switch (ref##target) {
        | `GitObject(target) => Js.Json.decodeString(target##oid)
        | _ => None
        }
      )
    ->getExn
  );
};

type state = RemoteData.t(unit, string, string);

type action =
  | CreateFork(string)
  | CreateBranch(string)
  | HandleResponse(string)
  | HandleFailure(string);

[@react.component]
let make =
    (~owner: string, ~name: string, ~onSuccess: string => unit, ~children) => {
  let currentUser = React.useContext(Context.CurrentUser.user);
  let currentUserLogin =
    currentUser->Context.CurrentUser.login->Belt.Option.getExn;

  let (state, send) =
    ReactUpdate.useReducer(RemoteData.NotAsked, (action, _state) =>
      switch (action) {
      | CreateFork(sha) =>
        UpdateWithSideEffects(
          Loading("Forking ..."),
          ({send}) => {
            Js.Promise.(
              Github_API_Client.createFork(~repo={j|$owner/$name|j})
              |> then_(_ => send(CreateBranch(sha)) |> resolve)
              |> ignore
            );
            None;
          },
        )
      | CreateBranch(sha) =>
        UpdateWithSideEffects(
          Loading("Branching ..."),
          ({send}) => {
            Js.Promise.(
              Github_API_Client.createBranch(
                ~repo={j|$currentUserLogin/$name|j},
                ~sha,
              )
              |> then_(newBranch =>
                   send(HandleResponse(newBranch)) |> resolve
                 )
              |> catch(error =>
                   send(HandleFailure(Js.String.make(error))) |> resolve
                 )
              |> ignore
            );
            None;
          },
        )
      | HandleResponse(newBranch) =>
        SideEffects(
          _self => {
            onSuccess(newBranch);
            None;
          },
        )
      | HandleFailure(error) => Update(Failure(error))
      }
    );
  switch (state) {
  | NotAsked =>
    let maybeForkQuery =
      Gql_Schema.MaybeFork.make(
        ~owner,
        ~name,
        ~qualifiedName={j|refs/heads/master|j},
        (),
      );
    <Gql_Query.MaybeFork variables=maybeForkQuery##variables>
      ...{({result}) =>
        switch (result) {
        | Loading =>
          <div> {"Loading viewer permission!" |> React.string} </div>
        | Error(error) => <div> {error##message |> React.string} </div>
        | Data(data) =>
          children(
            ~viewerPermission=data->viewerPermission,
            ~createFork=() => send(CreateFork(data->sha)),
            ~createBranch=() => send(CreateBranch(data->sha)),
          )
        }
      }
    </Gql_Query.MaybeFork>;
  | Loading(msg) => <div> {msg |> React.string} </div>
  | Success () => React.null
  | Failure(error) => <div> {error |> React.string} </div>
  };
};