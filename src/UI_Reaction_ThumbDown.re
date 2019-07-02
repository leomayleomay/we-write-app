[@react.component]
let make = (~subjectId, ~numberOfThumbDowns, ~viewerHasReacted) => {
  let (reacted, setReacted) = React.useState(() => viewerHasReacted);
  let (count, setCount) = React.useState(() => numberOfThumbDowns);

  let addReaction =
    Gql_Schema.AddReaction.make(~subjectId, ~content=`THUMBS_DOWN, ());

  let removeReaction =
    Gql_Schema.RemoveReaction.make(~subjectId, ~content=`THUMBS_DOWN, ());

  reacted
    ? <Gql_Mutation.RemoveReaction>
        ...{(mutation, _) =>
          <a
            className="button"
            onClick={_ =>
              Js.Promise.(
                mutation(~variables=removeReaction##variables, ())
                |> then_(_ => {
                     setReacted(_ => false);
                     setCount(count => count - 1);
                     resolve();
                   })
                |> ignore
              )
            }>
            <span className="icon">
              <i className="fas fa-thumbs-down" ariaHidden=true />
            </span>
            <span> {count->string_of_int->React.string} </span>
          </a>
        }
      </Gql_Mutation.RemoveReaction>
    : <Gql_Mutation.AddReaction>
        ...{(mutation, _) =>
          <a
            className="button"
            onClick={_ =>
              Js.Promise.(
                mutation(~variables=addReaction##variables, ())
                |> then_(_ => {
                     setReacted(_ => true);
                     setCount(count => count + 1);
                     resolve();
                   })
                |> ignore
              )
            }>
            <span className="icon">
              <i className="far fa-thumbs-down" ariaHidden=true />
            </span>
            <span> {count->string_of_int->React.string} </span>
          </a>
        }
      </Gql_Mutation.AddReaction>;
};