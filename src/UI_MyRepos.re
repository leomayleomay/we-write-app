module Index = {
  let updateQuery = (prev, next) =>
    Belt.Option.getWithDefault(
      next->ReasonApolloQuery.fetchMoreResultGet,
      prev,
    );
  [@react.component]
  let make = () => {
    let myReposQuery = Gql_Schema.MyRepos.make(~first=100, ());

    <Gql_Query.MyRepos variables=myReposQuery##variables>
      ...{({result, fetchMore}) =>
        switch (result) {
        | Loading =>
          <progress className="progress is-large is-info" max="100" />
        | Error(_) => React.null
        | Data(data) =>
          <>
            {data##viewer##repositories##nodes
             ->Belt.Option.getWithDefault([||])
             ->Belt.Array.keepMap(x => x)
             ->Utils.chunk(3)
             |> Array.mapi((i, chunk) =>
                  <div key={"row-" ++ string_of_int(i)} className="columns">
                    {chunk
                     |> Array.mapi((j, repo) =>
                          <div
                            key={"column-" ++ string_of_int(j)}
                            className="column is-one-third">
                            <UI_MyRepo_Card repo />
                          </div>
                        )
                     |> React.array}
                  </div>
                )
             |> React.array}
            {data##viewer##repositories##pageInfo##hasPreviousPage
               ? {
                 let moreReposQuery =
                   Gql_Schema.MyRepos.make(
                     ~last=100,
                     ~before=
                       Belt.Option.getExn(
                         data##viewer##repositories##pageInfo##startCursor,
                       ),
                     (),
                   );
                 <UI_Control.Button
                   text="Prev Page"
                   onClick={_ =>
                     fetchMore(
                       ~variables=moreReposQuery##variables,
                       ~updateQuery,
                       (),
                     )
                     |> ignore
                   }
                 />;
               }
               : React.null}
            {data##viewer##repositories##pageInfo##hasNextPage
               ? {
                 let moreReposQuery =
                   Gql_Schema.MyRepos.make(
                     ~first=100,
                     ~after=
                       Belt.Option.getExn(
                         data##viewer##repositories##pageInfo##endCursor,
                       ),
                     (),
                   );
                 <UI_Control.Button
                   text="Next Page"
                   onClick={_ =>
                     fetchMore(
                       ~variables=moreReposQuery##variables,
                       ~updateQuery,
                       (),
                     )
                     |> ignore
                   }
                 />;
               }
               : React.null}
          </>
        }
      }
    </Gql_Query.MyRepos>;
  };
};