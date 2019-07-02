module type Key = {let key: string;};

module Operation = (K: Key) => {
  open Dom.Storage;
  let key = K.key;
  let get = () => getItem(key, localStorage);
  let set = value => setItem(key, value, localStorage);
  let remove = () => removeItem(key, localStorage);
};