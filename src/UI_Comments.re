module New = {
  [@react.component]
  let make = (~subjectId: string, ~refetchQueries) => {
    let (body, setBody) = React.useState(() => "");

    React.useEffect1(
      () => {
        String.length(body) > 0
          ? OnBeforeUnload.register() : OnBeforeUnload.unregister();
        Some(() => OnBeforeUnload.unregister());
      },
      [|body|],
    );

    let addComment = Gql_Schema.AddComment.make(~subjectId, ~body, ());

    let currentUser = React.useContext(Context.CurrentUser.user);

    <Gql_Mutation.AddComment>
      ...{(mutate, {result}) =>
        <article className="media">
          <figure className="media-left">
            <p className="image is-64x64">
              {currentUser
               ->Context.CurrentUser.avatarUrl
               ->Belt.Option.mapWithDefault(
                   <img
                     src="https://bulma.io/images/placeholders/128x128.png"
                   />,
                   avatarUrl =>
                   <img src=avatarUrl />
                 )}
            </p>
          </figure>
          <div className="media-content">
            <div className="field">
              <div className="control">
                <textarea
                  className="textarea"
                  value=body
                  rows=3
                  disabled={result == Loading}
                  onChange={e => setBody(ReactEvent.Form.target(e)##value)}
                />
              </div>
            </div>
            <nav className="level">
              <div className="level-left">
                <div className="level-item">
                  <div className="field">
                    <div className="control">
                      <button
                        className={
                          result == Loading
                            ? "button is-link is-loading" : "button is-link"
                        }
                        onClick={_ =>
                          Js.Promise.(
                            mutate(
                              ~variables=addComment##variables,
                              ~refetchQueries,
                              (),
                            )
                            |> then_(_ => setBody(_ => "") |> resolve)
                            |> ignore
                          )
                        }>
                        {"Add Comment" |> React.string}
                      </button>
                    </div>
                  </div>
                </div>
              </div>
            </nav>
          </div>
        </article>
      }
    </Gql_Mutation.AddComment>;
  };
};