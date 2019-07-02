[@react.component]
let make = (~subjectId, ~reactionGroups) =>
  <div className="card-footer-item field is-grouped">
    {reactionGroups
     |> Array.map(reactionGroup =>
          <p className="control">
            <UI_Reaction_Group subjectId reactionGroup />
          </p>
        )
     |> React.array}
  </div>;