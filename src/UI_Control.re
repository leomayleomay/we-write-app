module TextInput = {
  [@react.component]
  let make = (~label: string, ~value: string, ~onChange) =>
    <div className="field">
      <label className="label"> {label |> React.string} </label>
      <div className="control">
        <input
          className="input"
          type_="text"
          value
          onChange={e => onChange(ReactEvent.Form.target(e)##value)}
        />
      </div>
    </div>;
};

module TextArea = {
  [@react.component]
  let make = (~label: string, ~rows: int, ~value: string, ~onChange) => {
    <div className="field">
      <label className="label"> {label |> React.string} </label>
      <div className="control">
        <textarea
          className="textarea"
          value
          rows
          onChange={e => onChange(ReactEvent.Form.target(e)##value)}
        />
      </div>
    </div>;
  };
};

module Button = {
  [@react.component]
  let make = (~text: string, ~onClick) => {
    <div className="field">
      <div className="control">
        <button className="button is-link is-fullwidth is-outlined" onClick>
          {text |> React.string}
        </button>
      </div>
    </div>;
  };
};