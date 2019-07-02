module Index = {
  type state = RemoteData.t(array(Model_Repo.t), unit, string);
  type action =
    | Load
    | HandleResponse(array(Model_Repo.t))
    | HandleFailure(string);
  [@react.component]
  let make = () => {
    let (state, send) =
      ReactUpdate.useReducer(RemoteData.NotAsked, (action, _state) =>
        switch (action) {
        | Load =>
          UpdateWithSideEffects(
            Loading(),
            ({send}) => {
              Js.Promise.(
                API_Client.getRepositories()
                |> then_(response =>
                     send(HandleResponse(Model_Repo.decode(response##data)))
                     |> resolve
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
    | Success(repos) =>
      repos->Utils.chunk(3)
      |> Array.mapi((i, chunk) =>
           <div key={"row-" ++ string_of_int(i)} className="columns">
             {chunk
              |> Array.mapi((j, repo) =>
                   <div
                     key={"column-" ++ string_of_int(j)}
                     className="column is-one-third">
                     <UI_Repo_Card repo />
                   </div>
                 )
              |> React.array}
           </div>
         )
      |> React.array
    | Failure(error) => error |> React.string
    };
  };
};