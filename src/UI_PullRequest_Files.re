module Index = {
  [%bs.raw {|require('react-diff-view/style/index.css')|}];

  type state = RemoteData.t(string, unit, string);
  type action =
    | Load
    | HandleResponse(string)
    | HandleFailure(string);
  [@react.component]
  let make = (~owner, ~name, ~number) => {
    let (state, send) =
      ReactUpdate.useReducer(RemoteData.NotAsked, (action, _state) =>
        switch (action) {
        | Load =>
          UpdateWithSideEffects(
            Loading(),
            ({send}) => {
              Js.Promise.(
                Github_API_Client.getPullRequestFiles(
                  ~repo={j|$owner/$name|j},
                  ~number,
                )
                |> then_(response =>
                     send(HandleResponse(response##data)) |> resolve
                   )
                |> catch(error =>
                     send(HandleFailure(Js.String.make(error))) |> resolve
                   )
                |> ignore
              );
              None;
            },
          )
        | HandleResponse(repos) => Update(Success(repos))
        | HandleFailure(error) => Update(Failure(error))
        }
      );
    React.useEffect0(() => {
      send(Load);
      None;
    });
    switch (state) {
    | NotAsked => React.null
    | Loading () =>
      <progress className="progress is-large is-info" max="100" />
    | Success(diff) =>
      <>
        <UI_PullRequest_Reviews.New owner name number />
        {UI_ReactDiffView.(
           parseDiff(diff)
           |> Array.map(file =>
                <Diff
                  key={file##oldRevision ++ " - " ++ file##newRevision}
                  viewType="split"
                  diffType=file##_type
                  hunks=file##hunks>
                  ...{hunks =>
                    hunks
                    |> Array.map(hunk => <Hunk key={hunk##content} hunk />)
                    |> React.array
                  }
                </Diff>
              )
         )
         |> React.array}
      </>
    | Failure(error) =>
      <div className="notification is-danger"> {error |> React.string} </div>
    };
  };
};